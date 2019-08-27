#pragma once
#ifndef BS_PLATFORM_PLATFORM_H
#define BS_PLATFORM_PLATFORM_H

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

#include "WindowTypes.h"
#include "events/Event.h"

namespace basalt::platform {

using PlatformEventCallback = std::function<void(const Event&)>;

void startup(const Config& config);

/*
 * \brief Shuts down the platform.
 *
 * Destroys the main window.
 */
void Shutdown();

void AddEventListener(PlatformEventCallback callback);
auto PollEvents() -> bool;
auto WaitForEvents() -> bool;
void RequestQuit();
auto GetName() -> std::string_view;

/**
 * \brief Retrieves the command line arguments as an argv style vector.
 *
 * This function returns the UTF-8 encoded argument list with which the
 * application and engine were originally called with. The program name
 * (usually argv[0]) is NOT included.
 *
 * \return the arguments.
 */
auto GetArgs() -> const std::vector<std::string>&;

auto get_window_data() -> const WindowData&;

} // namespace basalt::platform

#endif // !BS_PLATFORM_PLATFORM_H
