#pragma once
#ifndef BS_PLATFORM_H
#define BS_PLATFORM_H

/** \file
 * \brief Platform abstractions.
 *
 * This file contains a platform independant interface for interacting with the
 * underlying platform.
 */

#include <basalt/Config.h>
#include "IGfxContext.h"
#include "Types.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace basalt::platform {

struct Event;
using PlatformEventCallback = std::function<void(const Event&)>;

void startup(const Config& config);

/*
 * \brief Shuts down the platform.
 *
 * Destroys the main window.
 */
void shutdown();

void add_event_listener(const PlatformEventCallback& callback);

[[nodiscard]]
auto poll_events() -> std::vector<std::shared_ptr<Event>>;

[[nodiscard]]
auto wait_for_events() -> std::vector<std::shared_ptr<Event>>;

[[nodiscard]]
auto get_name() -> std::string_view;

/**
 * \brief Retrieves the command line arguments as an argv style vector.
 *
 * This function returns the UTF-8 encoded argument list with which the
 * application and engine were originally called with. The program name
 * (usually argv[0]) is NOT included.
 *
 * \return the arguments.
 */
[[nodiscard]]
auto get_args() -> const std::vector<std::string>&;

//auto get_window_title() -> std::string;
//void set_window_title(std::string_view windowTitle);

[[nodiscard]]
auto get_window_size() -> math::Vec2i32;

[[nodiscard]]
auto get_window_mode() -> WindowMode;
void set_window_mode(WindowMode windowMode);

[[nodiscard]]
auto get_window_gfx_context() -> IGfxContext*;

} // namespace basalt::platform

#endif // !BS_PLATFORM_H
