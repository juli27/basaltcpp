#pragma once

namespace basalt {

struct Platform final {
  [[nodiscard]] static auto is_debugger_attached() -> bool;
};

} // namespace basalt
