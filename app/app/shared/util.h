#pragma once
#ifndef BASALT_APP_SHARED_UTIL_H
#define BASALT_APP_SHARED_UTIL_H

#include <string>
#include <string_view>

namespace basalt::win32 {

auto create_wide_from_utf8(std::string_view src) -> std::wstring;

} // namespace basalt::win32

#endif // BASALT_APP_SHARED_UTIL_H
