#pragma once

#include "Windows_custom.h"

#include <basalt/api/shared/types.h>

#include <string>
#include <string_view>

namespace basalt {

[[nodiscard]]
auto create_wide_from_utf8(std::string_view src) -> std::wstring;

[[nodiscard]]
auto create_utf8_from_wide(std::wstring_view src) noexcept -> std::string;

namespace win32 {

auto get_size_u16(RECT const&) -> Size2Du16;

}

} // namespace basalt
