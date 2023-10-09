#pragma once

#include <basalt/api/gfx/backend/command_list.h>

#include <type_traits>

namespace basalt::gfx {

// implementations of engine private function templates

template <typename T>
auto CommandList::allocate(uSize const count) const -> gsl::span<T> {
  static_assert(std::is_trivially_destructible_v<T>);

  auto allocator = std::pmr::polymorphic_allocator<T>{mBuffer.get()};
  auto* const storage = allocator.allocate(count);

  return gsl::span{storage, count};
}

template <typename T, typename... Args>
auto CommandList::add(Args&&... args) -> void {
  static_assert(std::is_base_of_v<Command, T>,
                "CommandList only accepts commands derived from Command");

  mCommands.emplace_back(new (allocate<T>().data())
                           T(std::forward<Args>(args)...));
}

} // namespace basalt::gfx
