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
 * \param commandLine the windows command line string.
 * \param showCommand the show command for the first window.
 */
void Init(HINSTANCE instance, const WCHAR* commandLine, int showCommand);

/**
 * \brief Returns the Windows API handle for the main window.
 *
 * \return the main window handle.
 */
HWND GetWindowHandle();

/**
 * \brief Converts a UTF-8 encoded string into a wide string.
 *
 * The returned string is meant to be consumed by Windows API functions.

 * TODO: make noexcept
 *
 * \param source the UTF-8 string to convert.
 * \return the new wide string.
 */
std::wstring CreateWideFromUTF8(const std::string_view source);

} // namespace winapi
} // namespace platform
} // namespace basalt

#endif // !BS_PLATFORM_WINDOWS_API_H
