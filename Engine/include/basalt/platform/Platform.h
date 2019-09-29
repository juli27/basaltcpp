#pragma once
#ifndef BS_PLATFORM_H
#define BS_PLATFORM_H

/** \file
 * \brief Platform abstractions.
 *
 * This file contains a platform independant interface for interacting with the
 * underlying platform.
 */

#include <functional>
#include <string>
#include <vector>

#include <basalt/Config.h>

#include "IGfxContext.h"
#include "Types.h"

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
auto poll_events() -> bool;
auto wait_for_events() -> bool;
void request_quit();
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
auto get_args() -> const std::vector<std::string>&;

//auto get_window_title() -> std::string;
//void set_window_title(std::string_view windowTitle);

auto get_window_size() -> math::Vec2i32;
auto get_window_mode() -> WindowMode;
void set_window_mode(WindowMode windowMode);
auto get_window_gfx_context() -> IGfxContext*;

} // namespace basalt::platform

#endif // !BS_PLATFORM_H
