#include "runtime/shared/win32/Util.h"

#include "runtime/shared/win32/Win32APIHeader.h"

#include <limits>

using std::numeric_limits;
using std::string_view;
using std::wstring;

namespace basalt {

auto create_wide_from_utf8(const string_view src) noexcept -> wstring {
  // Don't use asserts/log because this function is used before the log
  // is initialized

  // MultiByteToWideChar fails when size is 0
  if (src.empty()) {
    return {};
  }

  // use the size of the string view because the input string
  // can be non null-terminated
  if (src.size() > static_cast<string_view::size_type>(
    numeric_limits<int>::max())) {
    return L"create_wide_from_utf8: input string is too large";
  }

  const auto srcSize = static_cast<int>(src.size());
  auto dstSize = ::MultiByteToWideChar(
    CP_UTF8, 0, src.data(), srcSize, nullptr, 0
  );
  if (dstSize == 0) {
    return L"create_wide_from_utf8: MultiByteToWideChar returned 0";
  }

  wstring dst(dstSize, L'\0');
  dstSize = ::MultiByteToWideChar(
    CP_UTF8, 0, src.data(), srcSize, dst.data(), static_cast<int>(dst.size())
  );
  if (dstSize == 0) {
    return L"create_wide_from_utf8: MultiByteToWideChar returned 0";
  }

  return dst;
}

} // namespace basalt
