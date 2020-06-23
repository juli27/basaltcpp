#include <runtime/platform/platform.h>

#include "globals.h"
#include "shared/Windows_custom.h"

#include <runtime/shared/log.h>

namespace basalt::platform {

auto get_window_mode() -> WindowMode {
  return sWindowMode;
}

void set_window_mode(const WindowMode windowMode) {
  switch (windowMode) {
  case WindowMode::Windowed:
    BASALT_LOG_ERROR(
      "platform::set_window_mode for Windowed not implemented");
    break;
  case WindowMode::Fullscreen:
    BASALT_LOG_ERROR(
      "platform::set_window_mode for Fullscreen not implemented");
    break;
  case WindowMode::FullscreenExclusive:
    BASALT_LOG_ERROR(
      "platform::set_window_mode for FullscreenExclusive not implemented");
    break;
  }
}

auto is_debugger_attached() -> bool {
  return ::IsDebuggerPresent();
}

} // namespace basalt::platform
