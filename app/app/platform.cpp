#include <api/platform/platform.h>

#include "shared/Windows_custom.h"

namespace basalt::platform {

auto is_debugger_attached() -> bool {
  return ::IsDebuggerPresent();
}

} // namespace basalt::platform
