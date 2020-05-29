#pragma once
#ifndef BASALT_PLATFORM_H
#define BASALT_PLATFORM_H

#include "runtime/shared/Config.h"

namespace basalt::platform {

[[nodiscard]]
auto get_window_mode() -> WindowMode;
void set_window_mode(WindowMode windowMode);

#if BASALT_DEV_BUILD

auto is_debugger_attached() -> bool;

#endif // BASALT_DEV_BUILD

} // namespace basalt::platform

#endif // !BASALT_PLATFORM_H
