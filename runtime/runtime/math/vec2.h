#pragma once

#include "runtime/shared/types.h"

namespace basalt {

template <typename T>
struct Vec2 {
  T x {};
  T y {};

  constexpr Vec2() noexcept = default;

  constexpr Vec2(const T xVal, const T yVal) noexcept : x {xVal}, y {yVal} {
  }

  [[nodiscard]] constexpr auto operator==(const Vec2& other) const noexcept
    -> bool {
    return this->x == other.x && this->y == other.y;
  }

  [[nodiscard]] constexpr auto operator!=(const Vec2& other) const noexcept
    -> bool {
    return !(*this == other);
  }

  void set(T ax, T ay) noexcept;
};

extern template struct Vec2<i32>;

using Vec2i32 = Vec2<i32>;

} // namespace basalt
