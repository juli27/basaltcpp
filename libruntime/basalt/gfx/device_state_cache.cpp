#include <basalt/gfx/device_state_cache.h>

namespace basalt::gfx {

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

auto DeviceStateCache::update(const IndexBuffer handle) noexcept -> bool {
  if (handle != mBoundIndexBuffer) {
    mBoundIndexBuffer = handle;
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
                              const Matrix4x4f32& transform) noexcept -> bool {
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
                              const Color& emissive, const Color& specular,
                              const f32 specularPower) noexcept -> bool {
  if (!mMaterial || diffuse != mMaterial->diffuse ||
      ambient != mMaterial->ambient || emissive != mMaterial->emissive ||
      specular != mMaterial->specular ||
      specularPower != mMaterial->specularPower) {
    mMaterial = {diffuse, ambient, emissive, specular, specularPower};

    return true;
  }

  return false;
}

auto DeviceStateCache::update_fog_parameters(const Color& color,
                                             const f32 start, const f32 end,
                                             const f32 density) -> bool {
  if (!mFogParams || color != mFogParams->color || start != mFogParams->start ||
      end != mFogParams->end || density != mFogParams->density) {
    mFogParams = {color, start, end, density};

    return true;
  }

  return false;
}

auto DeviceStateCache::update_blend_constant(const Color& c) -> bool {
  if (mBlendConstant != c) {
    mBlendConstant = c;

    return true;
  }

  return false;
}

auto DeviceStateCache::update_reference_alpha(const u8 alpha) -> bool {
  if (mReferenceAlpha != alpha) {
    mReferenceAlpha = alpha;

    return true;
  }

  return false;
}

} // namespace basalt::gfx
