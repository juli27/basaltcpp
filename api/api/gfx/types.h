#pragma once

#include "backend/types.h"

#include "api/resources/types.h"
#include "api/math/mat4.h"

#include "api/shared/color.h"

#include "api/base/enum_array.h"
#include "api/base/types.h"

#include <entt/entity/entity.hpp>

#include <memory>

namespace basalt::gfx {

struct DrawTarget;

struct Drawable;
using DrawablePtr = std::shared_ptr<Drawable>;

struct ResourceCache;

struct RenderComponent final {
  MeshHandle mesh;
  Texture texture {entt::null};
  Material material {entt::null};
  Color diffuseColor;
  Color ambientColor;
  Mat4f32 texTransform {Mat4f32::identity()};
  TexCoordinateSrc tcs {TcsVertex};
  u8 renderFlags = RenderFlagNone;
};

static_assert(sizeof(RenderComponent) == 112);

struct Model final {
  GfxModel handle {entt::null};
};

static_assert(sizeof(Model) == 4);

struct MaterialData final {
  EnumArray<TextureStageState, u32, TEXTURE_STAGE_STATE_COUNT>
    textureStageStates {};
};

} // namespace basalt::gfx
