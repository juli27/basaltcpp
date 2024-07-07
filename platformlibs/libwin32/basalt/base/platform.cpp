#include <basalt/api/base/platform.h>

#include <basalt/win32/shared/Windows_custom.h>

namespace basalt {

auto Platform::is_debugger_attached() -> bool {
  return IsDebuggerPresent();
}

auto Platform::quit() -> void {
  PostQuitMessage(0);
}

} // namespace basalt
