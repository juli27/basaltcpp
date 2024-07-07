#include "message_queue.h"

#include "util.h"

#include <basalt/win32/shared/Windows_custom.h>

#include <basalt/api/shared/asserts.h>
#include <basalt/api/shared/log.h>

#include <memory>
#include <optional>

namespace basalt {

using std::nullopt;
using std::optional;

namespace {

thread_local Win32MessageQueue* threadMessageQueue = nullptr;

}

auto Win32MessageQueue::get_for_current_thread() -> Win32MessageQueuePtr {
  if (!threadMessageQueue) {
    if (!IsGUIThread(TRUE)) {
      BASALT_CRASH("failed to create win32 message queue");
    }

    return std::make_shared<Win32MessageQueue>();
  }

  return threadMessageQueue->shared_from_this();
}

Win32MessageQueue::Win32MessageQueue() : mThreadId{GetCurrentThreadId()} {
  threadMessageQueue = this;
}

Win32MessageQueue::~Win32MessageQueue() noexcept {
  BASALT_ASSERT(mThreadId == GetCurrentThreadId());

  threadMessageQueue = nullptr;
}

auto Win32MessageQueue::take() const -> MSG {
  BASALT_ASSERT(mThreadId == GetCurrentThreadId());

  auto message = MSG{};
  auto const result = GetMessageW(&message, nullptr, 0, 0);
  if (result == -1) {
    BASALT_LOG_FATAL(create_win32_error_message(GetLastError()));

    BASALT_CRASH("GetMessageW failed");
  }

  return message;
}

auto Win32MessageQueue::peek() const -> std::optional<MSG> {
  BASALT_ASSERT(mThreadId == GetCurrentThreadId());

  auto message = MSG{};
  if (!PeekMessageW(&message, nullptr, 0, 0, PM_NOREMOVE)) {
    return nullopt;
  }

  return message;
}

auto Win32MessageQueue::poll() const -> std::optional<MSG> {
  BASALT_ASSERT(mThreadId == GetCurrentThreadId());

  auto message = MSG{};
  if (!PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
    return nullopt;
  }

  return message;
}

} // namespace basalt
