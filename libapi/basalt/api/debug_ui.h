#pragma once

#include "gfx/info.h"
#include "gfx/types.h"

#include "shared/types.h"

#include "math/types.h"

#include "base/types.h"

namespace basalt {

class DebugUi {
public:
  auto show_gfx_info(gfx::Info const&) -> void;
  auto show_performance_overlay(bool& isOpen) -> void;

  static auto edit_directional_light(gfx::DirectionalLight&) -> void;
  static auto edit_color3(char const* label, Color&) -> bool;
  static auto edit_color4(char const* label, Color&) -> bool;
  static auto display_matrix4x4(char const* label, Matrix4x4f32 const&) -> void;

private:
  u32 mSelectedAdapterIndex{0};
  uSize mSelectedModeIndex{};
  u8 mOverlayCorner{2u};
};

} // namespace basalt
