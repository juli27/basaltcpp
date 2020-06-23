#pragma once

#include <runtime/shared/types.h>

namespace basalt {

template <typename T>
struct Vec2 {
  T x {};
  T y {};

  constexpr Vec2() noexcept = default;

  constexpr Vec2(const T x, const T y) noexcept
    : x {x}, y {y} {
  }

  constexpr Vec2(const Vec2&) noexcept = default;
  constexpr Vec2(Vec2&&) noexcept = default;

  ~Vec2() noexcept = default;

  auto operator=(const Vec2&) noexcept -> Vec2& = default;
  auto operator=(Vec2&&) noexcept -> Vec2& = default;

  [[nodiscard]]
  constexpr auto operator==(const Vec2& other) const noexcept -> bool {
    return this->x == other.x && this->y == other.y;
  }

  [[nodiscard]]
  constexpr auto operator!=(const Vec2& other) const noexcept -> bool {
    return !(*this == other);
  }

  void set(T ax, T ay) noexcept;
};

extern template struct Vec2<i32>;

using Vec2i32 = Vec2<i32>;

} // namespace basalt
