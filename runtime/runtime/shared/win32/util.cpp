#include "runtime/shared/win32/util.h"

#include "runtime/shared/win32/Windows_custom.h"

#include <limits>

using std::numeric_limits;
using std::string_view;
using std::wstring;
using namespace std::string_literals;

namespace basalt::win32 {

auto create_wide_from_utf8(const string_view src) -> wstring {
  // Don't use asserts/log because this function can be used without the log
  // being initialized

  // MultiByteToWideChar fails when size is 0
  if (src.empty()) {
    return {};
  }

  // use the size of the string view because the input string
  // can be non null-terminated
  using SizeType = string_view::size_type;
  if (src.size() > static_cast<SizeType>(numeric_limits<int>::max())) {
    return L"create_wide_from_utf8: input string is too large"s;
  }

  const auto srcSize = static_cast<int>(src.size());
  auto dstSize = ::MultiByteToWideChar(
    CP_UTF8, 0, src.data(), srcSize, nullptr, 0
  );
  if (dstSize == 0) {
    return L"create_wide_from_utf8: MultiByteToWideChar returned 0"s;
  }

  wstring dst(dstSize, L'\0');
  dstSize = ::MultiByteToWideChar(
    CP_UTF8, 0, src.data(), srcSize, dst.data(), static_cast<int>(dst.size())
  );
  if (dstSize == 0) {
    return L"create_wide_from_utf8: MultiByteToWideChar returned 0"s;
  }

  return dst;
}

} // namespace basalt::win32
