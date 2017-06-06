// Copyright 2017 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef COBALT_RENDERER_RASTERIZER_EGL_DRAW_RECT_SHADOW_BLUR_H_
#define COBALT_RENDERER_RASTERIZER_EGL_DRAW_RECT_SHADOW_BLUR_H_

#include <vector>

#include "cobalt/math/rect_f.h"
#include "cobalt/render_tree/color_rgba.h"
#include "cobalt/renderer/rasterizer/egl/draw_depth_stencil.h"
#include "cobalt/renderer/rasterizer/egl/draw_object.h"

namespace cobalt {
namespace renderer {
namespace rasterizer {
namespace egl {

// Example CSS box shadow (outset):
//   +-------------------------------------+
//   | Box shadow "blur" region            |
//   |   +-----------------------------+   |
//   |   | Box shadow "spread" region  |   |
//   |   |   +---------------------+   |   |
//   |   |   | Box shadow rect     |   |   |
//   |   |   | (exclude scissor)   |   |   |
//   |   |   +---------------------+   |   |
//   |   |                             |   |
//   |   +-----------------------------+   |
//   | (include scissor)                   |
//   +-------------------------------------+
// NOTE: Despite the CSS naming, the actual blur effect starts inside the
// "spread" region.

// Handles drawing a box shadow with blur. This uses a gaussian kernel to fade
// the "blur" region. A stencil is used to ensure only the desired pixels are
// touched.
//
// This uses a shader to mimic skia's SkBlurMask.cpp.
// See also http://stereopsis.com/shadowrect/ as reference for the formula
// used to approximate the gaussian integral (which controls the opacity of
// the shadow).
class DrawRectShadowBlur : public DrawObject {
 public:
  // Draw a blurred box shadow.
  // The box shadow exists in the area between |inner_rect| and |outer_rect|.
  // |blur_edge| specifies the area where the "spread" region transitions
  //   to the "blur" region. It has 50% opacity of the shadow color.
  DrawRectShadowBlur(GraphicsState* graphics_state,
                     const BaseState& base_state,
                     const math::RectF& inner_rect,
                     const math::RectF& outer_rect,
                     const math::RectF& blur_edge,
                     const render_tree::ColorRGBA& color,
                     const math::RectF& exclude_scissor,
                     float blur_sigma, bool inset);

  void ExecuteOnscreenUpdateVertexBuffer(GraphicsState* graphics_state,
      ShaderProgramManager* program_manager) OVERRIDE;
  void ExecuteOnscreenRasterize(GraphicsState* graphics_state,
      ShaderProgramManager* program_manager) OVERRIDE;

 private:
  void AddVertex(float x, float y, uint32_t color);

  struct VertexAttributes {
    float position[3];
    uint32_t color;
    float blur_position[2];   // Expressed in terms of blur_sigma from center.
  };

  DrawDepthStencil draw_stencil_;
  std::vector<VertexAttributes> attributes_;
  math::PointF blur_center_;
  float blur_radius_[2];      // Expressed in terms of blur_sigma.
  float blur_scale_add_[2];

  // The sigma scale is used to transform pixel distances to sigma-relative
  // distances.
  float blur_sigma_scale_;

  uint8_t* vertex_buffer_;
};

}  // namespace egl
}  // namespace rasterizer
}  // namespace renderer
}  // namespace cobalt

#endif  // COBALT_RENDERER_RASTERIZER_EGL_DRAW_RECT_SHADOW_BLUR_H_