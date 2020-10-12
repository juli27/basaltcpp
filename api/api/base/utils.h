#pragma once

#include <type_traits>

namespace basalt {

template <typename E, std::enable_if_t<std::is_enum_v<E>, int> = 0>
constexpr auto enum_cast(const E e) noexcept {
  return static_cast<std::underlying_type_t<E>>(e);
}

} // namespace basalt
