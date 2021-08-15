#pragma once

#include <basalt/api/gfx/backend/types.h>

#include <memory>
#include <memory_resource>
#include <type_traits>
#include <vector>

namespace basalt::gfx {

// serialized commands which the gfx device should execute
struct CommandList final {
  CommandList();

  CommandList(const CommandList&) = delete;
  CommandList(CommandList&&) noexcept = default;

  ~CommandList() noexcept = default;

  auto operator=(const CommandList&) -> CommandList& = delete;
  auto operator=(CommandList&&) -> CommandList& = default;

  [[nodiscard]] auto commands() const noexcept -> const std::vector<Command*>&;

  template <typename T, typename... Args>
  void add(Args&&... args) {
    static_assert(std::is_base_of_v<Command, T>,
                  "CommandList only accepts commands derived from Command");
    static_assert(std::is_trivially_destructible_v<T>);

    mCommands.emplace_back(::new (mBuffer->allocate(sizeof(T), alignof(T)))
                             T(std::forward<Args>(args)...));
  }

private:
  using CommandBuffer = std::pmr::monotonic_buffer_resource;

  std::unique_ptr<CommandBuffer> mBuffer;
  std::vector<Command*> mCommands;
};

} // namespace basalt::gfx
