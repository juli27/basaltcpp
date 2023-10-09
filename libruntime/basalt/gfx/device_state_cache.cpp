#include <basalt/gfx/device_state_cache.h>

namespace basalt::gfx {

auto DeviceStateCache::update(Pipeline const handle) noexcept -> bool {
  if (handle != mBoundPipeline) {
    mBoundPipeline = handle;

    return true;
  }

  return false;
}

auto DeviceStateCache::update(VertexBuffer const buffer,
                              u64 const offset) noexcept -> bool {
  if (buffer != mBoundVertexBuffer || offset != mVertexBufferOffset) {
    mBoundVertexBuffer = buffer;
    mVertexBufferOffset = offset;

    return true;
  }

  return false;
}

auto DeviceStateCache::update(IndexBuffer const handle) noexcept -> bool {
  if (handle != mBoundIndexBuffer) {
    mBoundIndexBuffer = handle;
    return true;
  }

  return false;
}

auto DeviceStateCache::update(u8 const slot, Sampler const sampler) noexcept
  -> bool {
  if (sampler != mBoundSamplers[slot]) {
    mBoundSamplers[slot] = sampler;
    return true;
  }

  return false;
}

auto DeviceStateCache::update(u8 const slot, Texture const texture) noexcept
  -> bool {
  if (texture != mBoundTextures[slot]) {
    mBoundTextures[slot] = texture;
    return true;
  }

  return false;
}

auto DeviceStateCache::update(TransformState const state,
                              Matrix4x4f32 const& transform) noexcept -> bool {
  auto& currentValue = mTransforms[state];
  if (currentValue != transform) {
    currentValue = transform;
    return true;
  }

  return false;
}

auto DeviceStateCache::update_ambient_light(Color const& c) noexcept -> bool {
  if (mAmbientLight != c) {
    mAmbientLight = c;

    return true;
  }

  return false;
}

auto DeviceStateCache::update(Color const& diffuse, Color const& ambient,
                              Color const& emissive, Color const& specular,
                              f32 const specularPower) noexcept -> bool {
  if (!mMaterial || diffuse != mMaterial->diffuse ||
      ambient != mMaterial->ambient || emissive != mMaterial->emissive ||
      specular != mMaterial->specular ||
      specularPower != mMaterial->specularPower) {
    mMaterial = {diffuse, ambient, emissive, specular, specularPower};

    return true;
  }

  return false;
}

auto DeviceStateCache::update_fog_parameters(Color const& color,
                                             f32 const start, f32 const end,
                                             f32 const density) -> bool {
  if (!mFogParams || color != mFogParams->color || start != mFogParams->start ||
      end != mFogParams->end || density != mFogParams->density) {
    mFogParams = {color, start, end, density};

    return true;
  }

  return false;
}

auto DeviceStateCache::update_blend_constant(Color const& c) -> bool {
  if (mBlendConstant != c) {
    mBlendConstant = c;

    return true;
  }

  return false;
}

auto DeviceStateCache::update_reference_alpha(u8 const alpha) -> bool {
  if (mReferenceAlpha != alpha) {
    mReferenceAlpha = alpha;

    return true;
  }

  return false;
}

} // namespace basalt::gfx
