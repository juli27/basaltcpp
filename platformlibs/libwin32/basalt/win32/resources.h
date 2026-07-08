#pragma once

#include "shared/Windows_custom.h"

namespace basalt::win32 {

auto load_big_icon(HINSTANCE, LPCWSTR name, UINT loadFlags) -> HICON;
auto load_small_icon(HINSTANCE, LPCWSTR name, UINT loadFlags) -> HICON;
auto load_cursor(HINSTANCE, LPCWSTR name, UINT loadFlags) -> HCURSOR;

} // namespace basalt::win32
