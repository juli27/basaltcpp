#include "utils.h"

#include "Windows_custom.h"

#include <basalt/api/base/types.h>

#include <limits>
#include <string>
#include <string_view>

using namespace std::literals;

using std::numeric_limits;
using std::string;
using std::string_view;
using std::wstring;
using std::wstring_view;

namespace basalt {

auto create_wide_from_utf8(string_view const src) -> wstring {
  // Don't use asserts/log because this function can be used without the log
  // being initialized

  // MultiByteToWideChar fails when size is 0
  if (src.empty()) {
    return wstring{};
  }

  // use the size of the string view because the input string
  // can be non null-terminated
  if (src.size() > static_cast<uSize>(numeric_limits<int>::max())) {
    return L"create_wide_from_utf8: input string is too large"s;
  }

  auto const numChars = static_cast<int>(src.size());
  auto numWChars =
    MultiByteToWideChar(CP_UTF8, 0ul, src.data(), numChars, nullptr, 0);
  if (numWChars == 0) {
    return L"create_wide_from_utf8: MultiByteToWideChar returned 0"s;
  }

  auto dst = wstring(numWChars, L'\0');
  numWChars = MultiByteToWideChar(CP_UTF8, 0ul, src.data(), numChars,
                                  dst.data(), static_cast<int>(dst.size()));
  if (numWChars == 0) {
    return L"create_wide_from_utf8: MultiByteToWideChar returned 0"s;
  }

  return dst;
}

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

} // namespace basalt
