#pragma once

#include <basalt/api/gfx/types.h>

namespace basalt {

class DebugUi final {
public:
  auto show_gfx_info(const gfx::Info&) -> void;

private:
  u32 mSelectedAdapter {};
};

} // namespace basalt
