#include "message_queue.h"

#include "util.h"

#include "shared/Windows_custom.h"

#include <basalt/api/base/asserts.h>
#include <basalt/api/base/log.h>

#include <memory>
#include <optional>

namespace basalt {

namespace {

thread_local std::unique_ptr<Win32MessageQueue> sThreadMessageQueue;

} // namespace

auto Win32MessageQueue::make_for_current_thread() -> Win32MessageQueue* {
  BASALT_ASSERT(!sThreadMessageQueue);
  
  if (auto const didConvert = IsGUIThread(TRUE);
      didConvert == ERROR_NOT_ENOUGH_MEMORY || !didConvert) {
    BASALT_CRASH("failed to convert thread to GUI thread");
  }
  sThreadMessageQueue = std::make_unique<Win32MessageQueue>();

  return sThreadMessageQueue.get();
}

auto Win32MessageQueue::get_for_current_thread() -> Win32MessageQueue* {
  auto* messageQueue = sThreadMessageQueue.get();
  BASALT_ASSERT(messageQueue);

  return messageQueue;
}

Win32MessageQueue::Win32MessageQueue() : mThreadId{GetCurrentThreadId()} {
}

auto Win32MessageQueue::take() -> MSG {
  BASALT_ASSERT(mThreadId == GetCurrentThreadId());

  auto message = MSG{};
  if (auto const result = GetMessageW(&message, nullptr, 0, 0); result == -1) {
    BASALT_LOG_FATAL(create_win32_error_message(GetLastError()));
    BASALT_CRASH("GetMessageW failed");
  }

  return message;
}

auto Win32MessageQueue::peek() const -> std::optional<MSG> {
  BASALT_ASSERT(mThreadId == GetCurrentThreadId());

  auto message = MSG{};
  if (!PeekMessageW(&message, nullptr, 0, 0, PM_NOREMOVE)) {
    return std::nullopt;
  }

  return message;
}

auto Win32MessageQueue::poll() -> std::optional<MSG> {
  BASALT_ASSERT(mThreadId == GetCurrentThreadId());

  auto message = MSG{};
  if (!PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
    return std::nullopt;
  }

  return message;
}

} // namespace basalt
