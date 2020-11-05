#include <api/platform/platform.h>

#include "shared/Windows_custom.h"

namespace basalt {

auto Platform::is_debugger_attached() -> bool {
  return IsDebuggerPresent();
}

} // namespace basalt
