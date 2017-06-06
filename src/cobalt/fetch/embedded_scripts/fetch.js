'use strict';(function(e){function n(a){"string"!==typeof a&&(a=String(a));if(/[^a-z0-9\-#$%&'*+.\^_`|~]/i.test(a))throw new g("Invalid character in header field name");return a.toLowerCase()}function x(a){"string"!==typeof a&&(a=String(a));var b;var c=0;for(b=a.length;c<b;c++){var d=a.charCodeAt(c);if(9!==d&&10!==d&&13!==d&&32!==d)break}for(b=a.length-1;b>c&&(d=a.charCodeAt(b),9===d||10===d||13===d||32===d);b--);a=a.substring(c,b+1);c=0;for(b=a.length;c<b;c++)if(d=a.charCodeAt(c),256<=d||0===d||
10===d||13===d)throw new g("Invalid character in header field value");return a}function f(a){this.map=new r;if(void 0!==a){if(null===a||"object"!==typeof a)throw new g("Constructing Headers with invalid parameters");a instanceof f?a.forEach(function(a,c){this.append(c,a)},this):m.isArray(a)?a.forEach(function(a){if(2!==a.length)throw new g("Constructing Headers with invalid parameters");this.append(a[0],a[1])},this):Object.getOwnPropertyNames(a).forEach(function(b){this.append(b,a[b])},this)}}function t(a){if(a.bodyUsed)return u(new g("Body already read"));
if(null===a.body)return C(new k(0));if(D(a.body))return u(new g("ReadableStream already locked"));var b=a.body.getReader(),c=[],d=0;return b.read().then(function p(a){if(a.done){if(0===c.length)a=new k(0);else if(1===c.length)a=new k(c[0]);else{a=new k(d);for(var e=0,f=c.length,h=0;e<f;e++)a.set(c[e],h),h+=c[e].length}return a}return a.value instanceof k?(d+=a.value.length,c.push(a.value),b.read().then(p)):u(new g("Invalid stream read value type"))})}function E(a){a=unescape(encodeURIComponent(a));
for(var b=new k(a.length),c=0,d=a.length;c<d;c++)b[c]=a.charCodeAt(c);return b}function y(){this._initBody=function(a){this._bodyUsed=!1;this.body=null===a||void 0===a?null:a instanceof v?a:new v({start:function(b){if(a)if("string"===typeof a)b.enqueue(E(a));else if(z.prototype.isPrototypeOf(a))b.enqueue(new k(a));else if(F(a))b.enqueue(new k(a.buffer));else throw new g("Unsupported BodyInit type");b.close()}});this.headers.get("content-type")||"string"===typeof a&&this.headers.set("content-type",
"text/plain;charset=UTF-8")};Object.defineProperty(this,"bodyUsed",{get:function(){return this._bodyUsed?!0:this.body?!!G(this.body):!1}});this.arrayBuffer=function(){return t(this).then(function(a){return a.buffer})};this.text=function(){return t(this).then(function(a){return 0===a.length?"":decodeURIComponent(escape(String.fromCharCode.apply(null,a)))})};this.json=function(){return this.text().then(JSON.parse)};return this}function q(a,b){b=b||{};var c=b.body;if(a instanceof q){if(a.bodyUsed)throw new g("Request body already read");
this.url=a.url;this.credentials=a.credentials;b.headers||(this.headers=new f(a.headers));this.method=a.method;this.mode=a.mode;c||null===a.body||(c=a.body,a._bodyUsed=!0)}else this.url=String(a);this.credentials=b.credentials||this.credentials||"omit";if(b.headers||!this.headers)this.headers=new f(b.headers);a=b.method||this.method||"GET";var d=a.toUpperCase();this.method=-1<H.indexOf(d)?d:a;this.mode=b.mode||this.mode||null;this.referrer=null;if(("GET"===this.method||"HEAD"===this.method)&&c)throw new g("Body not allowed for GET or HEAD requests");
this._initBody(c)}function I(a){var b=new f;a.replace(/\r?\n[\t ]+/g," ").split(/\r?\n/).forEach(function(a){var c=a.split(":");if(a=c.shift().trim())c=c.join(":").trim(),b.append(a,c)});return b}function l(a,b){b||(b={});this.type="default";this.status="status"in b?b.status:200;if(200>this.status||599<this.status)throw new A("Invalid response status");this.ok=200<=this.status&&300>this.status;if("statusText"in b){var c=b.statusText;for(var d=0,e=c.length,p;d<e;d++)if(p=c.charCodeAt(d),9!==p&&(32>
p||255<p||127===p))throw g("Invalid status text");}else c="OK";this.statusText=c;this.headers=new f(b.headers);this.url=b.url||"";if(a&&-1<J.indexOf(this.status))throw new g("Body not allowed with a null body status");this._initBody(a)}if(!e.fetch){var m=e.Array,z=e.ArrayBuffer,K=e.Symbol.iterator,r=e.Map,A=e.RangeError,g=e.TypeError,k=e.Uint8Array,w=e.Promise,u=w.reject,C=w.resolve,v=e.ReadableStream,B=e.ReadableStreamTee,G=e.IsReadableStreamDisturbed,D=e.IsReadableStreamLocked,L="[object Int8Array];[object Uint8Array];[object Uint8ClampedArray];[object Int16Array];[object Uint16Array];[object Int32Array];[object Uint32Array];[object Float32Array];[object Float64Array]".split(";"),
F=z.isView||function(a){return a&&-1<L.indexOf(Object.prototype.toString.call(a))};f.prototype.append=function(a,b){if(2!==arguments.length)throw g("Invalid parameters to append");a=n(a);b=x(b);this.map.has(a)?this.map.set(a,this.map.get(a)+", "+b):this.map.set(a,b)};f.prototype["delete"]=function(a){if(1!==arguments.length)throw g("Invalid parameters to delete");this.map.delete(n(a))};f.prototype.get=function(a){if(1!==arguments.length)throw g("Invalid parameters to get");a=n(a);var b=this.map.get(a);
return void 0!==b?b:null};f.prototype.has=function(a){if(1!==arguments.length)throw g("Invalid parameters to has");return this.map.has(n(a))};f.prototype.set=function(a,b){if(2!==arguments.length)throw g("Invalid parameters to set");this.map.set(n(a),x(b))};f.prototype.forEach=function(a,b){var c=this;m.from(this.map.entries()).sort().forEach(function(d){a.call(b,d[1],d[0],c)})};f.prototype.keys=function(){return(new r(m.from(this.map.entries()).sort())).keys()};f.prototype.values=function(){return(new r(m.from(this.map.entries()).sort())).values()};
f.prototype.entries=function(){return(new r(m.from(this.map.entries()).sort())).entries()};f.prototype[K]=f.prototype.entries;var H="DELETE GET HEAD OPTIONS POST PUT".split(" ");q.prototype.clone=function(){var a=null;null!==this.body&&(a=B(this.body,!0),this.body=a[0],a=a[1]);return new q(this,{body:a})};y.call(q.prototype);var J=[101,204,205,304];y.call(l.prototype);l.prototype.clone=function(){var a=null;null!==this.body&&(a=B(this.body,!0),this.body=a[0],a=a[1]);return new l(a,{status:this.status,
statusText:this.statusText,headers:new f(this.headers),url:this.url})};l.error=function(){var a=new l(null);a.type="error";a.status=0;a.statusText="";return a};var M=[301,302,303,307,308];l.redirect=function(a,b){if(!FetchInternal.IsUrlValid(a))throw new g("Invalid URL");void 0===b&&(b=302);if(-1===M.indexOf(b))throw new A("Invalid status code");return new l(null,{status:b,headers:{location:a}})};e.Headers=f;e.Request=q;e.Response=l;e.fetch=function(a,b){return new w(function(c,d){var e=!1,f=new q(a,
b),h=new XMLHttpRequest,k=null,n=new v({start:function(a){k=a},cancel:function(a){e=!0;h.abort()}});h.onload=function(){k.close()};h.onreadystatechange=function(){if(h.readyState===h.HEADERS_RECEIVED){var a={status:h.status,statusText:h.statusText,headers:I(h.getAllResponseHeaders()||"")};a.url="responseURL"in h?h.responseURL:a.headers.get("X-Request-URL");c(new l(n,a))}};h.onerror=function(){k.error(new g("Network request failed"));d(new g("Network request failed"))};h.ontimeout=function(){k.error(new g("Network request failed"));
d(new g("Network request failed"))};h.open(f.method,f.url,!0);"include"===f.credentials&&(h.withCredentials=!0);f.headers.forEach(function(a,b){h.setRequestHeader(b,a)});var m=function(a){e||k.enqueue(a)};null===f.body?h.fetch(m,null):t(f).then(function(a){h.fetch(m,a)})})};e.fetch.polyfill=!0}})(this);