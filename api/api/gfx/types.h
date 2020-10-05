#pragma once

#include "backend/types.h"

#include "api/math/mat4.h"

#include "api/shared/color.h"
#include "api/shared/types.h"

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
