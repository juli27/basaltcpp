#include <basalt/api/gfx/device_state_cache.h>

#include <basalt/api/gfx/backend/render_state.h>

namespace basalt::gfx {

DeviceStateCache::DeviceStateCache() noexcept {
  mTextureStates[TextureStageState::TextureTransformFlags] = TtfDisabled;
}

auto DeviceStateCache::update(const RenderState& renderState) noexcept -> bool {
  if (auto& currentValue = mRenderStates[renderState.type()];
      currentValue != renderState.value()) {
    currentValue = renderState.value();
    return true;
  }

  return false;
}

auto DeviceStateCache::update(const Pipeline handle) noexcept -> bool {
  if (handle != mBoundPipeline) {
    mBoundPipeline = handle;

    return true;
  }

  return false;
}

auto DeviceStateCache::update(const VertexBuffer buffer,
                              const u64 offset) noexcept -> bool {
  if (buffer != mBoundVertexBuffer || offset != mVertexBufferOffset) {
    mBoundVertexBuffer = buffer;
    mVertexBufferOffset = offset;

    return true;
  }

  return false;
}

auto DeviceStateCache::update(const Sampler sampler) noexcept -> bool {
  if (sampler != mBoundSampler) {
    mBoundSampler = sampler;
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

auto DeviceStateCache::update(const TransformState state,
                              const Mat4f32& transform) noexcept -> bool {
  auto& currentValue = mTransforms[state];
  if (currentValue != transform) {
    currentValue = transform;
    return true;
  }

  return false;
}

auto DeviceStateCache::update_ambient_light(const Color& c) noexcept -> bool {
  if (mAmbientLight != c) {
    mAmbientLight = c;

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

auto DeviceStateCache::update(const TextureStageState state,
                              const u32 value) noexcept -> bool {
  auto& currentValue = mTextureStates[state];
  if (currentValue != value) {
    currentValue = value;
    return true;
  }

  return false;
}

} // namespace basalt::gfx
