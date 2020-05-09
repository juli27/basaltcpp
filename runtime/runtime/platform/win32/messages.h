#pragma once
#ifndef BASALT_RUNTIME_PLATFORM_WIN32_MESSAGES_H
#define BASALT_RUNTIME_PLATFORM_WIN32_MESSAGES_H

#include "runtime/shared/win32/Windows_custom.h"

#include <string>

namespace basalt::win32 {

std::string message_to_string(UINT message, WPARAM wParam, LPARAM lParam);

} // namespace basalt::win32

#endif // BASALT_RUNTIME_PLATFORM_WIN32_MESSAGES_H
