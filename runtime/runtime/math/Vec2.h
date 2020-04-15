#pragma once
#ifndef BASALT_MATH_VEC2_H
#define BASALT_MATH_VEC2_H

#include "runtime/shared/Types.h"

#include <array>

namespace basalt::math {

template <typename T>
struct Vec2 final {
  constexpr Vec2() noexcept = default;
  constexpr Vec2(T x, T y) noexcept : mData{x, y} {}
  constexpr Vec2(const Vec2&) noexcept = default;
  constexpr Vec2(Vec2&&) noexcept = default;
  ~Vec2() noexcept = default;

  auto operator=(const Vec2&) noexcept -> Vec2& = default;
  auto operator=(Vec2&&) noexcept -> Vec2& = default;

  constexpr auto operator==(const Vec2& other) const noexcept -> bool {
    return this->x() == other.x() && this->y() == other.y();
  }

  constexpr auto operator!=(const Vec2& other) noexcept -> bool {
    return !(*this == other);
  }

  void set(T x, T y) noexcept;

  void set_x(T x) noexcept;
  void set_y(T y) noexcept;

  [[nodiscard]]
  constexpr auto x() const noexcept -> T {
    return std::get<0>(mData);
  }

  [[nodiscard]]
  constexpr auto y() const noexcept -> T {
    return std::get<1>(mData);
  }

private:
  std::array<T, 2> mData{};
};

extern template struct Vec2<i32>;
extern template struct Vec2<f32>;
extern template void Vec2<i32>::set(i32 x, i32 y) noexcept;
extern template void Vec2<f32>::set(f32 x, f32 y) noexcept;
extern template void Vec2<i32>::set_x(i32 x) noexcept;
extern template void Vec2<f32>::set_x(f32 x) noexcept;
extern template void Vec2<i32>::set_y(i32 y) noexcept;
extern template void Vec2<f32>::set_y(f32 y) noexcept;

using Vec2i32 = Vec2<i32>;
using Vec2f32 = Vec2<f32>;

} // namespace basalt::math

#endif // BASALT_MATH_VEC2_H
