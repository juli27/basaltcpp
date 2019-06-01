#pragma once
#ifndef BS_PLATFORM_WINDOWS_API_H
#define BS_PLATFORM_WINDOWS_API_H

#include <string>

#include "WindowsHeader.h"

namespace basalt {
namespace platform {
namespace winapi {

/*
 * \brief Initializes the Windows API platform.
 *
 * Saves the instance handle and show command and initializes the argument
 * vector for GetArgs().
 *
 * \param instance handle to the application instance.
 * \param cmdLine the windows command line string.
 * \param cmdShow the show command for the first window.
 */
void Init(HINSTANCE instance, const WCHAR* commandLine, int showCommand);

HWND GetWindowHandle();

std::wstring CreateWideFromUTF8(const std::string_view source);

} // namespace winapi
} // namespace platform
} // namespace basalt

#endif // !BS_PLATFORM_WINDOWS_API_H
