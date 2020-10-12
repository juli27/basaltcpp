#include "device_state_cache.h"

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

} // namespace basalt::gfx
