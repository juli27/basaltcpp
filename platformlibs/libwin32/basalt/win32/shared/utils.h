#pragma once

#include <string>
#include <string_view>

namespace basalt {

[[nodiscard]]
auto create_wide_from_utf8(std::string_view src) -> std::wstring;

[[nodiscard]]
auto create_utf8_from_wide(std::wstring_view src) noexcept -> std::string;

} // namespace basalt
