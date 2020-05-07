#pragma once
#ifndef BASALT_PLATFORM_H
#define BASALT_PLATFORM_H

#include "runtime/shared/Config.h"
#include "runtime/shared/Size2D.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace basalt {

namespace gfx::backend {
struct IGfxContext;
} // namespace gfx::backend

namespace platform {

struct Event;
using PlatformEventCallback = std::function<void(const Event&)>;

void startup(const Config& config);
void shutdown();
void add_event_listener(const PlatformEventCallback& callback);

[[nodiscard]]
auto poll_events() -> std::vector<std::shared_ptr<Event>>;

[[nodiscard]]
auto wait_for_events() -> std::vector<std::shared_ptr<Event>>;

[[nodiscard]]
auto get_name() -> std::string_view;

//auto get_window_title() -> std::string;
//void set_window_title(std::string_view windowTitle);

[[nodiscard]]
auto get_window_size() -> Size2Du16;

[[nodiscard]]
auto get_window_mode() -> WindowMode;
void set_window_mode(WindowMode windowMode);

[[nodiscard]]
auto get_window_gfx_context() -> gfx::backend::IGfxContext*;

} // namespace platform
} // namespace basalt

#endif // !BASALT_PLATFORM_H
