#pragma once

#include "backend/types.h"
#include "api/shared/types.h"

#include <array>

namespace basalt::gfx {

struct DeviceStateCache final {
  auto update(RenderState, u32) noexcept -> bool;

private:
  std::array<u32, RENDER_STATE_COUNT> mRenderStates {
    /* RenderState::Lighting */ true,
    /* RenderState::CullMode */ CullModeCcw};
};

} // namespace basalt::gfx
