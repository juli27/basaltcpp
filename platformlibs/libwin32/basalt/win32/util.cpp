#include <basalt/win32/util.h>

#include <basalt/api/shared/asserts.h>
#include <basalt/api/shared/log.h>

#include <basalt/api/base/platform.h>
#include <basalt/api/base/types.h>

#include <limits>
#include <string>

using namespace std::literals;

using std::numeric_limits;
using std::string;
using std::string_view;
using std::wstring;
using std::wstring_view;

namespace basalt {

/**
 * \brief Converts a Windows API wide string to UTF-8.
 *
 * The converted string is returned in a new string object.
 * Fails with std::terminate when string storage allocation throws.
 *
 * \param src the wide string to convert.
 * \return the wide string converted to UTF-8.
 */
auto create_utf8_from_wide(wstring_view const src) noexcept -> string {
  // Don't use asserts/log because this function is used before the log
  // is initialized

  // Do NOT call CreateWinAPIErrorMessage in this function
  // because it uses this function and may cause an infinite loop

  // TODO: noexcept allocator and heap memory pool for strings

  // WideCharToMultiByte fails when size is 0
  if (src.empty()) {
    return string{};
  }

  // use the size of the string view because the input string
  // can be non null-terminated
  if (src.size() > static_cast<uSize>(numeric_limits<int>::max())) {
    return u8"create_utf8_from_wide: string to convert is too large"s;
  }

  auto const srcSize = static_cast<int>(src.size());
  auto dstSize = WideCharToMultiByte(CP_UTF8, 0u, src.data(), srcSize, nullptr,
                                     0, nullptr, nullptr);

  if (dstSize == 0) {
    return u8"WideCharToMultiByte returned 0"s;
  }

  auto dst = string(dstSize, '\0');
  dstSize = WideCharToMultiByte(CP_UTF8, 0u, src.data(), srcSize, dst.data(),
                                static_cast<int>(dst.size()), nullptr, nullptr);
  if (dstSize == 0) {
    return u8"WideCharToMultiByte returned 0"s;
  }

  return dst;
}

/**
 * \brief Creates an error description from a Windows API error code.
 *
 * \param errorCode Windows API error code.
 * \return description string of the error.
 */
auto create_win32_error_message(DWORD const errorCode) noexcept -> string {
  WCHAR* buffer = nullptr;
  auto const numChars = FormatMessageW(
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS,
    nullptr, errorCode, 0u, reinterpret_cast<WCHAR*>(&buffer), 0u, nullptr);

  if (numChars == 0u) {
    return u8"FormatMessageW failed"s;
  }

  // use numChars because the buffer is NOT null terminated
  auto message = create_utf8_from_wide({buffer, numChars});

  LocalFree(buffer);

  return message;
}

auto load_system_cursor(WORD const id, int const width, int const height,
                        UINT const flags) noexcept -> HCURSOR {
  constexpr auto commonFlags = u32{LR_SHARED};

  return static_cast<HCURSOR>(LoadImageW(nullptr, MAKEINTRESOURCEW(id),
                                         IMAGE_CURSOR, width, height,
                                         commonFlags | flags));
}

#if BASALT_DEV_BUILD

namespace detail {

auto verify_win32_bool(BOOL const result) -> BOOL {
  if (!result) {
    BASALT_LOG_ERROR("win32 error: {}",
                     create_win32_error_message(GetLastError()));

    if (Platform::is_debugger_attached()) {
      BASALT_BREAK_DEBUGGER();
    }
  }

  return result;
}

} // namespace detail

#endif

} // namespace basalt
