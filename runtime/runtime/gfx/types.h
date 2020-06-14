#pragma once

#include "backend/render_command.h"
#include "backend/types.h"

#include <runtime/math/mat4.h>

#include <runtime/shared/color.h>
#include <runtime/shared/types.h>

namespace basalt::gfx {

struct RenderComponent final {
  MeshHandle mesh;
  TextureHandle texture;
  ModelHandle model;
  Color diffuseColor;
  Color ambientColor;
  Mat4f32 texTransform {Mat4f32::identity()};
  TexCoordinateSrc tcs {TexCoordinateSrc::Vertex};
  u8 renderFlags = RenderFlagNone;
};

// GfxComponent
// Geometry
// Materials
//
// RenderComponent
// list of parts/nodes
//   geometry
//   material

} // namespace basalt::gfx
