#include "runtime/platform/Platform.h"

#include "runtime/platform/win32/globals.h"
#include "runtime/platform/win32/util.h"

#include "runtime/shared/win32/Windows_custom.h"

#include "runtime/platform/events/Event.h"

#include "runtime/shared/Asserts.h"
#include "runtime/shared/Log.h"

#include <utility>

using std::shared_ptr;
using std::vector;

namespace basalt::platform {

using namespace win32;

void add_event_listener(const PlatformEventCallback& callback) {
  sEventListener.push_back(callback);
}

auto poll_events() -> vector<shared_ptr<Event>> {
  MSG msg{};
  while (::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
    ::TranslateMessage(&msg);
    ::DispatchMessageW(&msg);

    if (!msg.hwnd) {
      switch (msg.message) {
      case WM_QUIT:
        sPendingEvents.push_back(std::make_shared<QuitEvent>());
        break;

      default:
        // ‭275‬ is WM_TIMER
        // is received upon focus change
        BASALT_LOG_DEBUG("unhandled thread message: {}", msg.message);
        break;
      }
    }
  }

  return std::move(sPendingEvents);
}

auto wait_for_events() -> vector<shared_ptr<Event>> {
  MSG msg{};
  const auto ret = ::GetMessageW(&msg, nullptr, 0u, 0u);
  if (ret == -1) {
    BASALT_LOG_ERROR(create_winapi_error_message(::GetLastError()));
    // TODO: fixme
    BASALT_ASSERT_MSG(false, "::GetMessageW error");
  }

  // GetMessage retrieved WM_QUIT
  if (ret == 0) {
    sPendingEvents.push_back(std::make_shared<QuitEvent>());
    return sPendingEvents;
  }

  ::TranslateMessage(&msg);
  ::DispatchMessageW(&msg);

  // handle any remainig messages in the queue
  return poll_events();
}

auto get_window_size() -> Size2Du16 {
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

} // namespace basalt::platform
