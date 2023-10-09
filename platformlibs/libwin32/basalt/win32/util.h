#pragma once

#include <basalt/win32/shared/Windows_custom.h>

#include <string>

namespace basalt {

auto create_utf8_from_wide(std::wstring_view) noexcept -> std::string;

auto create_win32_error_message(DWORD errorCode) noexcept -> std::string;

// see LoadImageW documentation
[[nodiscard]] auto load_system_cursor(WORD id, int width, int height,
                                      UINT flags) noexcept -> HCURSOR;

#if BASALT_DEV_BUILD

namespace detail {

auto verify_win32_bool(BOOL) -> BOOL;

} // namespace detail

#endif

} // namespace basalt

#if BASALT_DEV_BUILD

#define VERIFY_WIN32_BOOL(win32bool)                                           \
  ::basalt::detail::verify_win32_bool(win32bool)

#else

#define VERIFY_WIN32_BOOL(win32bool) (win32bool)

#endif
