#pragma once
#ifndef BASALT_PLATFORM_H
#define BASALT_PLATFORM_H

#include "runtime/shared/Config.h"
#include "runtime/shared/Size2D.h"

#include <functional>
#include <memory>
#include <vector>

namespace basalt::platform {

struct Event;
using PlatformEventCallback = std::function<void(const Event&)>;

void add_event_listener(const PlatformEventCallback& callback);

[[nodiscard]]
auto poll_events() -> std::vector<std::shared_ptr<Event>>;

[[nodiscard]]
auto wait_for_events() -> std::vector<std::shared_ptr<Event>>;

//auto get_window_title() -> std::string;
//void set_window_title(std::string_view windowTitle);

[[nodiscard]]
auto get_window_size() -> Size2Du16;

[[nodiscard]]
auto get_window_mode() -> WindowMode;
void set_window_mode(WindowMode windowMode);

} // namespace basalt::platform

#endif // !BASALT_PLATFORM_H
