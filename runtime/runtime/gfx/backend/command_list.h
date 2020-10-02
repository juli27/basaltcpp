#pragma once

#include "types.h"

#include <memory>
#include <type_traits>
#include <vector>

namespace basalt::gfx {

using CommandPtr = std::unique_ptr<Command>;

// associates commands with their common transform (camera) and
// defines defaults for render state flags (lighting on/off, ...)
// (TODO: can every state flag be overridden by each command
//        or only some, or none)
struct CommandList final {
  CommandList() noexcept = default;

  CommandList(const CommandList&) = delete;
  CommandList(CommandList&&) noexcept = default;

  ~CommandList() noexcept = default;

  auto operator=(const CommandList&) -> CommandList& = delete;
  auto operator=(CommandList &&) -> CommandList& = default;

  [[nodiscard]] auto commands() const noexcept
    -> const std::vector<CommandPtr>&;

  template <typename T, typename... Args>
  void add(Args&&... args) {
    static_assert(std::is_base_of_v<Command, T>,
                  "CommandLists only accept commands derived from Command");
    static_assert(std::is_trivially_destructible_v<T>);

    mCommands.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
  }

private:
  std::vector<CommandPtr> mCommands {};
};

} // namespace basalt::gfx
