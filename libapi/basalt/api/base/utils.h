#pragma once

#include <algorithm>
#include <limits>
#include <type_traits>

namespace basalt {

template <typename E, std::enable_if_t<std::is_enum_v<E>, int> = 0>
constexpr auto enum_cast(const E e) noexcept {
  return static_cast<std::underlying_type_t<E>>(e);
}

// TODO: optimize unsigned -> unsigned conversion (same min value)
template <typename T, typename U>
constexpr auto saturated_cast(const U value) -> T {
  using Limits = std::numeric_limits<T>;

  return static_cast<T>(
    std::clamp(value, U {Limits::min()}, U {Limits::max()}));
}

} // namespace basalt
