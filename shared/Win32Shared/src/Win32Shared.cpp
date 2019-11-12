#include "basalt/shared/Win32APIHeader.h"

#include <cstddef>
#include <limits>
#include <string>

using std::numeric_limits;
using std::size_t;
using std::string_view;
using std::wstring;

auto create_wide_from_utf8(const string_view src) noexcept -> wstring {
  // Don't use asserts/log because this function is used before the log
  // is initialized

  // TODO: noexcept allocator and heap memory pool for strings

  // MultiByteToWideChar fails when size is 0
  if (src.empty()) {
    return {};
  }

  // use the size of the string view because the input string
  // can be non null-terminated
  if (src.size() > static_cast<size_t>(numeric_limits<int>::max())) {
    return L"create_wide_from_utf8: string to convert is too large";
  }

  const auto srcSize = static_cast<int>(src.size());
  auto dstSize = ::MultiByteToWideChar(
    CP_UTF8, 0, src.data(), srcSize, nullptr, 0
  );

  if (dstSize == 0) {
    return L"MultiByteToWideChar returned 0";
  }

  wstring dst(dstSize, L'\0');
  dstSize = ::MultiByteToWideChar(
    CP_UTF8, 0, src.data(), srcSize, dst.data(), static_cast<int>(dst.size())
  );

  if (dstSize == 0) {
    return L"MultiByteToWideChar returned 0";
  }

  return dst;
}
