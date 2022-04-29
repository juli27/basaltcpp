#pragma once

#include <basalt/api/gfx/types.h>

#include <basalt/api/base/types.h>

namespace basalt {

class DebugUi final {
public:
  auto show_gfx_info(const gfx::Info&) -> void;

private:
  u32 mSelectedAdapterIndex {0};
};

} // namespace basalt
