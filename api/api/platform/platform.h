#pragma once

#include "api/shared/types.h"

namespace basalt::platform {

[[nodiscard]] auto get_window_mode() -> WindowMode;
void set_window_mode(WindowMode windowMode);

auto is_debugger_attached() -> bool;

} // namespace basalt::platform
