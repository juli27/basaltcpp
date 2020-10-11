#pragma once

#include "backend/types.h"

#include "api/base/types.h"

#include <array>

namespace basalt::gfx {

struct DeviceStateCache final {
  auto update(RenderState, u32) noexcept -> bool;
  auto update(TextureStageState, u32 value) noexcept -> bool;

private:
  std::array<u32, RENDER_STATE_COUNT> mRenderStates {
    /* RenderState::CullMode */ CullModeCcw,
    /* RenderState::Ambient  */ 0,
    /* RenderState::Lighting */ true,
  };
  std::array<u32, TEXTURE_STAGE_STATE_COUNT> mTextureStates {
    /* TextureStageState::CoordinateSource */ 0,
  };
};

} // namespace basalt::gfx
