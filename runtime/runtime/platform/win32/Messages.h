#pragma once
#ifndef BASALT_PLATFORM_WIN32_MESSAGES_H
#define BASALT_PLATFORM_WIN32_MESSAGES_H

#include "runtime/shared/win32/Win32APIHeader.h"

#include <string>

namespace basalt {

std::string message_to_string(UINT message, WPARAM wParam, LPARAM lParam);

} // namespace basalt

#endif // BASALT_PLATFORM_WIN32_MESSAGES_H
