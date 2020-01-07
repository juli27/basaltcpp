#include <basalt/shared/Types.h>

#include <basalt/shared/Win32APIHeader.h>

#include <limits>
#include <string>

using std::numeric_limits;
using std::string;
using std::string_view;
using std::wstring;
using std::wstring_view;

using basalt::uSize;

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
auto create_utf8_from_wide(const wstring_view src) noexcept -> string {
  // Don't use asserts/log because this function is used before the log
  // is initialized

  // Do NOT call CreateWinAPIErrorMessage in this function
  // because it uses this function and may cause an infinite loop

  // TODO: noexcept allocator and heap memory pool for strings

  // WideCharToMultiByte fails when size is 0
  if (src.empty()) {
    return {};
  }

  // use the size of the string view because the input string
  // can be non null-terminated
  if (src.size() > static_cast<uSize>(numeric_limits<int>::max())) {
    return u8"create_utf8_from_wide: string to convert is too large";
  }

  const auto srcSize = static_cast<int>(src.size());
  auto dstSize = ::WideCharToMultiByte(
    CP_UTF8, 0u, src.data(), srcSize, nullptr, 0, nullptr, nullptr
  );

  if (dstSize == 0) {
    return "WideCharToMultiByte returned 0";
  }

  string dst(dstSize, '\0');
  dstSize = ::WideCharToMultiByte(
    CP_UTF8, 0u, src.data(), srcSize, dst.data(), static_cast<int>(dst.size()),
    nullptr, nullptr
  );
  if (dstSize == 0) {
    return "WideCharToMultiByte returned 0";
  }

  return dst;
}

/**
 * \brief Creates an error description from a Windows API error code.
 *
 * \param errorCode Windows API error code.
 * \return description string of the error.
 */
auto create_winapi_error_message(const DWORD errorCode) noexcept -> string {
  WCHAR* buffer = nullptr;
  const auto numChars = ::FormatMessageW(
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
    | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, errorCode, 0u,
    reinterpret_cast<WCHAR*>(&buffer), 0u, nullptr
  );

  if (numChars == 0u) {
    return "FormatMessageW failed";
  }

  // use numChars because the buffer is NOT null terminated
  const auto message = create_utf8_from_wide({buffer, numChars});

  ::LocalFree(buffer);

  return message;
}

} // namespace basalt
