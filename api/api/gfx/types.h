#pragma once

#include "backend/types.h"

#include "api/resources/types.h"
#include "api/math/mat4.h"

#include "api/shared/color.h"
#include "api/shared/types.h"

namespace basalt::gfx {

struct ResourceCache;

struct RenderComponent final {
  MeshHandle mesh;
  Texture texture {entt::null};
  Color diffuseColor;
  Color ambientColor;
  Mat4f32 texTransform {Mat4f32::identity()};
  TexCoordinateSrc tcs {TexCoordinateSrc::Vertex};
  u8 renderFlags = RenderFlagNone;
};

struct Model final {
  GfxModel handle {entt::null};
};

} // namespace basalt::gfx
