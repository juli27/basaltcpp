#pragma once
#ifndef BASALT_APP_UTIL_H
#define BASALT_APP_UTIL_H

#include "shared/Windows_custom.h"

#include <string>

namespace basalt::win32 {

auto create_utf8_from_wide(std::wstring_view src) noexcept -> std::string;
auto create_winapi_error_message(DWORD errorCode) noexcept -> std::string;

} // namespace basalt::win32

#endif // BASALT_APP_UTIL_H
