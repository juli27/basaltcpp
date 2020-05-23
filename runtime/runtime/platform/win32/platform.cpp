#include "runtime/platform/Platform.h"

#include "runtime/platform/win32/globals.h"

#if BASALT_DEV_BUILD
#include "runtime/shared/win32/Windows_custom.h"
#endif // BASALT_DEV_BUILD

#include "runtime/shared/Log.h"

namespace basalt::platform {

using namespace win32;

void add_event_listener(const PlatformEventCallback& callback) {
  sEventListener.push_back(callback);
}

auto window_size() -> Size2Du16 {
  return sWindowData.clientAreaSize;
}

auto get_window_mode() -> WindowMode {
  return sWindowData.mode;
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

#if BASALT_DEV_BUILD

auto is_debugger_attached() -> bool {
  return ::IsDebuggerPresent();
}

#endif // BASALT_DEV_BUILD

} // namespace basalt::platform
