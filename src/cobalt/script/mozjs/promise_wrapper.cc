/*
 * Copyright 2017 Google Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "cobalt/script/mozjs/promise_wrapper.h"

#include "base/logging.h"
#include "third_party/mozjs/js/src/jsfun.h"

namespace cobalt {
namespace script {
namespace mozjs {
namespace {
enum ReservedSlots {
  kResolveFunction,
  kRejectFunction,
  kPromiseObject,
  kNumReservedSlots,
};

JSClass native_promise_class = {
    "NativePromise",  // name

    JSCLASS_HAS_RESERVED_SLOTS(kNumReservedSlots),  // flags

    JS_PropertyStub,        // addProperty
    JS_DeletePropertyStub,  // delProperty
    JS_PropertyStub,        // getProperty
    JS_StrictPropertyStub,  // setProperty
    JS_EnumerateStub,       // enumerate
    JS_ResolveStub,         // resolve
    JS_ConvertStub,         // convert
    NULL,                   // finalize
    NULL,                   // trace
};

JSBool NativeExecutor(JSContext* context, unsigned argc, JS::Value* vp) {
  // Get the resolve/reject functions from the call args.
  JS::CallArgs call_args = CallArgsFromVp(argc, vp);
  DCHECK_EQ(call_args.length(), 2);

  // Get the this object. Should be the native_promise object.
  JS::RootedValue this_value(context, call_args.computeThis(context));
  DCHECK(this_value.isObject());
  JS::RootedObject this_object(context, JSVAL_TO_OBJECT(this_value));
  DCHECK_EQ(JS_GetClass(this_object), &native_promise_class);

  // First argument is the resolve function. Second is the reject function.
  // Stash these in the reserved slots. Reserved slots get visited so there is
  // no need to define a special trace function.
  JS::RootedValue resolve_function_value(context, call_args.get(0));
  JS::RootedValue reject_function_value(context, call_args.get(1));
  DCHECK(resolve_function_value.isObject());
  DCHECK(JS_ObjectIsFunction(context, JSVAL_TO_OBJECT(resolve_function_value)));
  DCHECK(reject_function_value.isObject());
  DCHECK(JS_ObjectIsFunction(context, JSVAL_TO_OBJECT(reject_function_value)));

  JS_SetReservedSlot(this_object, kResolveFunction, resolve_function_value);
  JS_SetReservedSlot(this_object, kRejectFunction, reject_function_value);
  return true;
}

// Creates a new NativePromise object and initializes its reserved slots.
JSObject* CreateNativePromise(JSContext* context) {
  JS::RootedObject native_promise(
      context, JS_NewObject(context, &native_promise_class, NULL, NULL));
  DCHECK(native_promise);
  for (uint32_t i = 0; i < kNumReservedSlots; ++i) {
    JS_SetReservedSlot(native_promise, i, JS::NullHandleValue);
  }
  return native_promise;
}

// Create a new native function with the |native_promise| bound as |this|.
JSObject* CreateExecutorArgument(JSContext* context,
                                 JS::HandleObject native_promise) {
  JS::RootedObject executor_function(context);
  executor_function =
      JS_NewFunction(context, &NativeExecutor, 2, 0, NULL, NULL);
  DCHECK(executor_function);

  JS::RootedObject bound_executor(context);
  bound_executor = JS_BindCallable(context, executor_function, native_promise);
  DCHECK(bound_executor);
  return bound_executor;
}

// Get the Promise constructor from the global object.
JSObject* GetPromiseConstructor(JSContext* context,
                                JS::HandleObject global_object) {
  JS::RootedValue promise_constructor_property(context);
  JSBool result = JS_GetProperty(context, global_object, "Promise",
                                 promise_constructor_property.address());
  DCHECK(result);
  if (!promise_constructor_property.isObject() ||
      !JS_ObjectIsFunction(context,
                           JSVAL_TO_OBJECT(promise_constructor_property))) {
    DLOG(ERROR) << "\"Promise\" property is not a function.";
    return NULL;
  }
  return JSVAL_TO_OBJECT(promise_constructor_property);
}

void Settle(JSContext* context, JS::HandleValue result,
            JS::HandleObject resolver, ReservedSlots slot) {
  JS::RootedValue slot_value(context, JS_GetReservedSlot(resolver, slot));
  DCHECK(slot_value.isObject());
  DCHECK(JS_ObjectIsFunction(context, JSVAL_TO_OBJECT(slot_value)));

  JS::RootedValue return_value(context);
  const size_t kNumArguments = result.isUndefined() ? 0 : 1;
  JS::Value args[1] = {result};
  JSBool call_result =
      JS_CallFunctionValue(context, resolver, slot_value, kNumArguments, args,
                           return_value.address());
  if (!call_result) {
    DLOG(ERROR) << "Exception calling Promise function.";
    JS_ClearPendingException(context);
  }
}
}  // namespace

JSObject* PromiseWrapper::Create(JSContext* context,
                                 JS::HandleObject global_object) {
  // Get the Promise constructor.
  JS::RootedObject constructor(context,
                               GetPromiseConstructor(context, global_object));
  if (!constructor) {
    DLOG(ERROR) << "Failed to find Promise constructor.";
    return NULL;
  }
  // Create a new NativePromise JS object, and bind it to the NativeExecutor
  // function.
  JS::RootedObject promise_wrapper(context, CreateNativePromise(context));
  DCHECK(promise_wrapper);
  JS::RootedObject executor(context,
                            CreateExecutorArgument(context, promise_wrapper));
  DCHECK(executor);

  // Invoke the Promise constructor with the native executor function.
  const size_t kNumArguments = 1;
  JS::Value args[kNumArguments] = {OBJECT_TO_JSVAL(executor)};
  JS::RootedObject promise_object(context);
  promise_object = JS_New(context, constructor, kNumArguments, args);
  if (!promise_object) {
    DLOG(ERROR) << "Failed to create a new Promise.";
    return NULL;
  }
  // Maintain a handle to the promise object on the NativePromise.
  JS_SetReservedSlot(promise_wrapper, kPromiseObject,
                     OBJECT_TO_JSVAL(promise_object));

  return promise_wrapper;
}

JSObject* PromiseWrapper::GetPromise() const {
  JS::RootedObject promise(context_);
  JS::RootedObject promise_wrapper(context_, weak_promise_wrapper_.GetObject());
  if (promise_wrapper) {
    JS::RootedValue slot_value(
        context_, JS_GetReservedSlot(promise_wrapper, kPromiseObject));
    DCHECK(slot_value.isObject());
    promise = JSVAL_TO_OBJECT(slot_value);
  }
  return promise;
}

void PromiseWrapper::Resolve(JS::HandleValue value) const {
  JS::RootedObject promise_wrapper(context_, weak_promise_wrapper_.GetObject());
  if (promise_wrapper) {
    Settle(context_, value, promise_wrapper, kResolveFunction);
  }
}

void PromiseWrapper::Reject(JS::HandleValue value) const {
  JS::RootedObject promise_wrapper(context_, weak_promise_wrapper_.GetObject());
  if (promise_wrapper) {
    Settle(context_, value, promise_wrapper, kRejectFunction);
  }
}

PromiseWrapper::PromiseWrapper(JSContext* context,
                               JS::HandleObject promise_wrapper)
    : context_(context), weak_promise_wrapper_(context, promise_wrapper) {
  DCHECK_EQ(JS_GetClass(promise_wrapper), &native_promise_class);
}

}  // namespace mozjs
}  // namespace script
}  // namespace cobalt