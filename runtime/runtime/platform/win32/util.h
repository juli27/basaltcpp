#pragma once
#ifndef BASALT_RUNTIME_PLATFORM_WIN32_UTIL_H
#define BASALT_RUNTIME_PLATFORM_WIN32_UTIL_H

#include "runtime/shared/win32/Windows_custom.h"

#include <string>

namespace basalt::win32 {

auto create_utf8_from_wide(std::wstring_view src) noexcept -> std::string;
auto create_winapi_error_message(DWORD errorCode) noexcept -> std::string;

} // namespace basalt::win32

#endif // BASALT_RUNTIME_PLATFORM_WIN32_UTIL_H
