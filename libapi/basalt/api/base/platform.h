#pragma once

namespace basalt {

struct Platform final {
  Platform() = delete;

  [[nodiscard]]
  static auto is_debugger_attached() -> bool;

  static auto quit() -> void;
};

} // namespace basalt
