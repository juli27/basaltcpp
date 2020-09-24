#pragma once

#include "backend/render_command.h"
#include "backend/types.h"

#include <runtime/math/mat4.h>

#include <runtime/shared/color.h>
#include <runtime/shared/types.h>

#include <string>

namespace basalt::gfx {

struct RenderComponent final {
  MeshHandle mesh;
  TextureHandle texture;
  Color diffuseColor;
  Color ambientColor;
  Mat4f32 texTransform {Mat4f32::identity()};
  TexCoordinateSrc tcs {TexCoordinateSrc::Vertex};
  u8 renderFlags = RenderFlagNone;
};

struct Model final {
  std::string model;
};

} // namespace basalt::gfx
