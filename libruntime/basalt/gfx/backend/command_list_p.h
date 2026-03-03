#pragma once

#include <basalt/api/gfx/backend/command_list.h> // IWYU pragma: export

#include "commands.h"

#include <basalt/api/base/types.h>

#include <gsl/span>

#include <memory_resource>
#include <type_traits>

namespace basalt::gfx {

class CommandListP {
public:
  template <typename T, typename... Args>
  static auto add(CommandList& cmdList, Args&&... args) -> void {
    static_assert(std::is_base_of_v<Command, T>,
                  "CommandList only accepts commands derived from Command");

    auto const storage = allocate<T>(cmdList);
    cmdList.mCommands.push_back(new (storage.data())
                                  T(std::forward<Args>(args)...));
  }

  template <typename T>
  [[nodiscard]]
  static auto allocate(CommandList& cmdList, uSize count = 1) -> gsl::span<T> {
    static_assert(std::is_trivially_destructible_v<T>);

    auto allocator = std::pmr::polymorphic_allocator<T>{cmdList.mBuffer.get()};
    auto* const storage = allocator.allocate(count);

    return gsl::span{storage, count};
  }
};

} // namespace basalt::gfx
