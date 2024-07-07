#pragma once

#include "types.h"

#include <basalt/win32/shared/Windows_custom.h>

#include <memory>
#include <optional>

namespace basalt {

class Win32MessageQueue final
  : public std::enable_shared_from_this<Win32MessageQueue> {
public:
  static auto get_for_current_thread() -> Win32MessageQueuePtr;

  Win32MessageQueue();

  Win32MessageQueue(Win32MessageQueue const&) = delete;
  Win32MessageQueue(Win32MessageQueue&&) = delete;

  ~Win32MessageQueue() noexcept;

  auto operator=(Win32MessageQueue const&) -> Win32MessageQueue& = delete;
  auto operator=(Win32MessageQueue&&) -> Win32MessageQueue& = delete;

  // blocks and dispatches sent messages until a posted message is available
  auto take() const -> MSG;

  auto peek() const -> std::optional<MSG>;

  auto poll() const -> std::optional<MSG>;

private:
  DWORD mThreadId;
};

} // namespace basalt
