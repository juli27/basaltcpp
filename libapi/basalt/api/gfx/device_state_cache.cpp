#include <basalt/api/gfx/device_state_cache.h>

namespace basalt::gfx {

DeviceStateCache::DeviceStateCache() {
  mRenderStates[RenderState::CullMode] = CullModeCcw;
  mRenderStates[RenderState::Ambient] = 0u;
  mRenderStates[RenderState::Lighting] = true;

  mTextureStates[TextureStageState::CoordinateSource] = TcsVertex;
  mTextureStates[TextureStageState::TextureTransformFlags] = TtfDisabled;
}

auto DeviceStateCache::update(const RenderState state, const u32 value) noexcept
  -> bool {
  auto& currentValue = mRenderStates[state];
  if (currentValue != value) {
    currentValue = value;
    return true;
  }

  return false;
}

auto DeviceStateCache::update(const TextureStageState state,
                              const u32 value) noexcept -> bool {
  auto& currentValue = mTextureStates[state];
  if (currentValue != value) {
    currentValue = value;
    return true;
  }

  return false;
}

auto DeviceStateCache::update(const TransformState state,
                              const Mat4f32& transform) noexcept -> bool {
  auto& currentValue = mTransforms[state];
  if (currentValue != transform) {
    currentValue = transform;
    return true;
  }

  return false;
}

auto DeviceStateCache::update(const Color& diffuse, const Color& ambient,
                              const Color& emissive) noexcept -> bool {
  if (diffuse != mMaterial.diffuse || ambient != mMaterial.ambient ||
      emissive != mMaterial.emissive) {
    mMaterial.diffuse = diffuse;
    mMaterial.ambient = ambient;
    mMaterial.emissive = emissive;

    return true;
  }

  return false;
}

auto DeviceStateCache::update(const Texture texture) noexcept -> bool {
  if (texture != mBoundTexture) {
    mBoundTexture = texture;
    return true;
  }

  return false;
}

} // namespace basalt::gfx
