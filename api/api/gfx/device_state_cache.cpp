#include "device_state_cache.h"

#include "api/shared/utils.h"

namespace basalt::gfx {

auto DeviceStateCache::update(const RenderState state, const u32 value) noexcept
  -> bool {
  auto& currentValue = mRenderStates[enum_cast(state)];
  if (currentValue != value) {
    currentValue = value;
    return true;
  }

  return false;
}

} // namespace basalt::gfx
