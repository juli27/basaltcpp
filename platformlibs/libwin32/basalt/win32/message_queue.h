#pragma once

#include "shared/Windows_custom.h"

#include <optional>

namespace basalt {

class Win32MessageQueue {
public:
  static auto ensure_for_current_thread() -> Win32MessageQueue&;
  static auto get_for_current_thread() -> Win32MessageQueue&;
  static auto has_for_current_thread() -> bool;

  Win32MessageQueue();

  Win32MessageQueue(Win32MessageQueue const&) = delete;
  Win32MessageQueue(Win32MessageQueue&&) = delete;

  ~Win32MessageQueue() noexcept = default;

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
