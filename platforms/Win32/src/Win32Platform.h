#pragma once
#ifndef BS_PLATFORM_WINDOWS_API_H
#define BS_PLATFORM_WINDOWS_API_H

#include "Win32APIHeader.h"

namespace basalt::platform::winapi {

/*
 * \brief Initializes the Windows API platform.
 *
 * Saves the instance handle and show command and initializes the argument
 * vector for GetArgs().
 *
 * \param instance handle to the application instance.
 * \param commandLine the windows command line string.
 * \param showCommand the show command for the first window.
 */
void init(HINSTANCE instance, const WCHAR* commandLine, int showCommand);

} // namespace basalt::platform::winapi

#endif // !BS_PLATFORM_WINDOWS_API_H
