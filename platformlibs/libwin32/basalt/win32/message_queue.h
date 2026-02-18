#pragma once

#include "types.h"

#include "shared/Windows_custom.h"

#include <optional>

namespace basalt {

class Win32MessageQueue {
public:
  static auto make_for_current_thread() -> Win32MessageQueuePtr;
  static auto get_for_current_thread() -> Win32MessageQueue*;

  Win32MessageQueue();

  Win32MessageQueue(Win32MessageQueue const&) = delete;
  Win32MessageQueue(Win32MessageQueue&&) = delete;

  ~Win32MessageQueue() noexcept;

  auto operator=(Win32MessageQueue const&) -> Win32MessageQueue& = delete;
  auto operator=(Win32MessageQueue&&) -> Win32MessageQueue& = delete;

  // blocks and dispatches sent messages until a posted message is available
  auto take() -> MSG;

  auto peek() const -> std::optional<MSG>;
  auto poll() -> std::optional<MSG>;

private:
  DWORD mThreadId;
};

} // namespace basalt
