#pragma once

#include "runtime/shared/types.h"

namespace basalt {

enum class WindowMode : u8;

namespace platform {

[[nodiscard]] auto get_window_mode() -> WindowMode;
void set_window_mode(WindowMode windowMode);

auto is_debugger_attached() -> bool;

} // namespace platform
} // namespace basalt
