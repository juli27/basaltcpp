#include <basalt/api/base/platform.h>

#include <basalt/win32utilities/Windows_custom.h>

namespace basalt {

auto Platform::is_debugger_attached() -> bool {
  return IsDebuggerPresent();
}

} // namespace basalt
