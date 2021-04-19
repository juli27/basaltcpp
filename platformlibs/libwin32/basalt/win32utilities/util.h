#pragma once

#include <string>
#include <string_view>

namespace basalt {

auto create_wide_from_utf8(std::string_view src) -> std::wstring;

} // namespace basalt
