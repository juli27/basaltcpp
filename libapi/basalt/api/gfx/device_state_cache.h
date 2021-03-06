#pragma once

#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/shared/color.h>

#include <basalt/api/math/mat4.h>

#include <basalt/api/base/enum_array.h>
#include <basalt/api/base/types.h>

namespace basalt::gfx {

struct DeviceStateCache final {
  DeviceStateCache();

  auto update(RenderState, u32) noexcept -> bool;
  auto update(TextureStageState, u32 value) noexcept -> bool;
  auto update(TransformState, const Mat4f32&) noexcept -> bool;
  auto update(const Color& diffuse, const Color& ambient,
              const Color& emissive) noexcept -> bool;
  auto update(Texture) noexcept -> bool;

private:
  struct Material final {
    Color diffuse;
    Color ambient;
    Color emissive;
  };

  EnumArray<RenderState, u32, RENDER_STATE_COUNT> mRenderStates;
  EnumArray<TextureStageState, u32, TEXTURE_STAGE_STATE_COUNT> mTextureStates;
  EnumArray<TransformState, Mat4f32, TRANSFORM_STATE_COUNT> mTransforms;
  Material mMaterial;
  Texture mBoundTexture {Texture::null()};
};

} // namespace basalt::gfx
