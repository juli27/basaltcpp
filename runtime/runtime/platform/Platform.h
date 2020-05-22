#pragma once
#ifndef BASALT_PLATFORM_H
#define BASALT_PLATFORM_H

#include "runtime/shared/Config.h"
#include "runtime/shared/Size2D.h"

#include <functional>

namespace basalt::platform {

struct Event;
using PlatformEventCallback = std::function<void(const Event&)>;

void add_event_listener(const PlatformEventCallback& callback);

[[nodiscard]]
auto get_window_size() -> Size2Du16;

[[nodiscard]]
auto get_window_mode() -> WindowMode;
void set_window_mode(WindowMode windowMode);

#if BASALT_DEV_BUILD

auto is_debugger_attached() -> bool;

#endif // BASALT_DEV_BUILD

} // namespace basalt::platform

#endif // !BASALT_PLATFORM_H
