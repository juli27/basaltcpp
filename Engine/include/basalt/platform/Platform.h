#pragma once
#ifndef BS_PLATFORM_PLATFORM_H
#define BS_PLATFORM_PLATFORM_H

/** \file
 * \brief Platform abstractions.
 *
 * This file contains the Platform class which abstracts the underlying
 * platform systems away (windowing, events, OS).
 */

#include <functional>
#include <string>
#include <vector>

#include "WindowTypes.h"
#include "events/Event.h"

namespace basalt {
namespace platform {

using PlatformEventCallback = std::function<void(const Event&)>;

void Startup(const WindowDesc& desc);

/*
 * \brief Shuts down the platform.
 *
 * Destroys the main window.
 */
void Shutdown();

void AddEventListener(PlatformEventCallback callback);

bool PollEvents();

void RequestQuit();

std::string_view GetName();

/**
 * \brief Retrieves the command line arguments as an argv style vector.
 *
 * This function returns the UTF-8 encoded argument list with which the
 * application and engine were originally called with. The program name 
 * (usually argv[0]) is NOT included.
 *
 * \return the arguments.
 */
const std::vector<std::string>& GetArgs();

const WindowDesc& GetWindowDesc();

} // namespace platform
} // namespace basalt

#endif // !BS_PLATFORM_PLATFORM_H
