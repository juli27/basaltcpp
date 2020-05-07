#pragma once
#ifndef BASALT_RUNTIME_SHARED_WIN32_UTIL_H
#define BASALT_RUNTIME_SHARED_WIN32_UTIL_H

#include <string>

namespace basalt::win32 {

auto create_wide_from_utf8(std::string_view src) -> std::wstring;

} // namespace basalt::win32

#endif // BASALT_RUNTIME_SHARED_WIN32_UTIL_H
