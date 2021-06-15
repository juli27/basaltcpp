#include <basalt/win32/shared/utils.h>

#include <basalt/win32/shared/Windows_custom.h>

#include <basalt/api/base/types.h>

#include <limits>

using namespace std::literals;

using std::numeric_limits;
using std::string_view;
using std::wstring;

namespace basalt {

auto create_wide_from_utf8(const string_view src) -> wstring {
  // Don't use asserts/log because this function can be used without the log
  // being initialized

  // MultiByteToWideChar fails when size is 0
  if (src.empty()) {
    return wstring {};
  }

  // use the size of the string view because the input string
  // can be non null-terminated
  if (src.size() > static_cast<uSize>(numeric_limits<int>::max())) {
    return L"create_wide_from_utf8: input string is too large"s;
  }

  const auto numChars = static_cast<int>(src.size());
  auto numWChars =
    ::MultiByteToWideChar(CP_UTF8, 0ul, src.data(), numChars, nullptr, 0);
  if (numWChars == 0) {
    return L"create_wide_from_utf8: MultiByteToWideChar returned 0"s;
  }

  wstring dst(numWChars, L'\0');
  numWChars = ::MultiByteToWideChar(CP_UTF8, 0ul, src.data(), numChars,
                                    dst.data(), static_cast<int>(dst.size()));
  if (numWChars == 0) {
    return L"create_wide_from_utf8: MultiByteToWideChar returned 0"s;
  }

  return dst;
}

} // namespace basalt
