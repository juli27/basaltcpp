#pragma once

#include <basalt/api/gfx/types.h>

namespace basalt {

class DebugUi final {
public:
  DebugUi() = delete;

  static auto show_gfx_info(const gfx::Info&) -> void;
};

} // namespace basalt
