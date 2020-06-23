#pragma once

#include "shared/Windows_custom.h"

#include <string>

namespace basalt {

auto create_utf8_from_wide(std::wstring_view src) noexcept -> std::string;
auto create_winapi_error_message(DWORD errorCode) noexcept -> std::string;

} // namespace basalt
