#include "message_queue.h"

#include "util.h"

#include "shared/Windows_custom.h"

#include <basalt/api/base/asserts.h>
#include <basalt/api/base/log.h>

#include <memory>
#include <optional>

namespace basalt {

namespace {

thread_local Win32MessageQueue* sThreadMessageQueue = nullptr;

} // namespace

auto Win32MessageQueue::make_for_current_thread() -> Win32MessageQueuePtr {
  BASALT_ASSERT(!sThreadMessageQueue);
  if (auto const didConvert = IsGUIThread(TRUE);
      didConvert == ERROR_NOT_ENOUGH_MEMORY || !didConvert) {
    BASALT_CRASH("failed to make GUI thread");
  }

  return std::make_shared<Win32MessageQueue>();
}

auto Win32MessageQueue::get_for_current_thread() -> Win32MessageQueue* {
  auto* messageQueue = sThreadMessageQueue;
  BASALT_ASSERT(messageQueue);

  return messageQueue;
}

Win32MessageQueue::Win32MessageQueue() : mThreadId{GetCurrentThreadId()} {
  sThreadMessageQueue = this;
}

Win32MessageQueue::~Win32MessageQueue() noexcept {
  BASALT_ASSERT(mThreadId == GetCurrentThreadId());
  sThreadMessageQueue = nullptr;
}

auto Win32MessageQueue::take() -> MSG {
  BASALT_ASSERT(mThreadId == GetCurrentThreadId());

  auto message = MSG{};
  if (auto const result = GetMessage(&message, nullptr, 0, 0); result == -1) {
    BASALT_LOG_FATAL(create_win32_error_message(GetLastError()));
    BASALT_CRASH("GetMessageW failed");
  }

  return message;
}

auto Win32MessageQueue::peek() const -> std::optional<MSG> {
  BASALT_ASSERT(mThreadId == GetCurrentThreadId());

  auto message = MSG{};
  if (!PeekMessage(&message, nullptr, 0, 0, PM_NOREMOVE)) {
    return std::nullopt;
  }

  return message;
}

auto Win32MessageQueue::poll() -> std::optional<MSG> {
  BASALT_ASSERT(mThreadId == GetCurrentThreadId());

  auto message = MSG{};
  if (!PeekMessage(&message, nullptr, 0, 0, PM_REMOVE)) {
    return std::nullopt;
  }

  return message;
}

} // namespace basalt
