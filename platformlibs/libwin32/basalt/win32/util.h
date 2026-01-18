#pragma once

#include <basalt/win32/shared/Windows_custom.h>

#include <string>

namespace basalt {

auto create_win32_error_message(DWORD errorCode) noexcept -> std::string;

#if BASALT_IS_DEV_BUILD
namespace detail {

auto verify_win32_bool(BOOL) -> BOOL;

} // namespace detail
#endif

} // namespace basalt

#if BASALT_IS_DEV_BUILD
#define VERIFY_WIN32_BOOL(win32bool)                                           \
  ::basalt::detail::verify_win32_bool(win32bool)
#else
#define VERIFY_WIN32_BOOL(win32bool) (win32bool)
#endif
