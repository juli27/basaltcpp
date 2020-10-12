#pragma once

#include "backend/types.h"

#include "api/base/enum_array.h"
#include "api/base/types.h"

namespace basalt::gfx {

struct DeviceStateCache final {
  DeviceStateCache();

  auto update(RenderState, u32) noexcept -> bool;
  auto update(TextureStageState, u32 value) noexcept -> bool;

private:
  EnumArray<RenderState, u32, RENDER_STATE_COUNT> mRenderStates;
  EnumArray<TextureStageState, u32, TEXTURE_STAGE_STATE_COUNT> mTextureStates;
};

} // namespace basalt::gfx
