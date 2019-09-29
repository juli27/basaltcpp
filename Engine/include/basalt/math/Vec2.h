#pragma once
#ifndef BS_MATH_VEC2_H
#define BS_MATH_VEC2_H

#include <basalt/common/Types.h>

namespace basalt::math {

template <typename T>
class Vec2 final {
public:
  constexpr Vec2() noexcept = default;
  constexpr Vec2(T x, T y) noexcept;
  constexpr Vec2(const Vec2&) noexcept = default;
  constexpr Vec2(Vec2&&) noexcept = default;
  ~Vec2() noexcept = default;

  auto operator=(const Vec2&) noexcept -> Vec2& = default;
  auto operator=(Vec2&&) noexcept -> Vec2& = default;
  constexpr auto operator==(const Vec2& other) noexcept -> bool;
  constexpr auto operator!=(const Vec2& other) noexcept -> bool;

  void set(T x, T y) noexcept;

  void set_x(T x) noexcept;
  void set_y(T y) noexcept;

  [[nodiscard]] constexpr auto get_x() const noexcept -> T;
  [[nodiscard]] constexpr auto get_y() const noexcept -> T;

private:
  T mX = {};
  T mY = {};
};

template<typename T>
constexpr Vec2<T>::Vec2(const T x, const T y) noexcept : mX(x), mY(y) {}

template <typename T>
constexpr auto Vec2<T>::operator==(const Vec2& other) noexcept -> bool {
  return this->mX == other.mX && this->mY == other.mY;
}

template <typename T>
constexpr auto Vec2<T>::operator!=(const Vec2& other) noexcept -> bool {
  return !(*this == other);
}

template<typename T>
void Vec2<T>::set(T x, T y) noexcept {
  mX = x;
  mY = y;
}

template<typename T>
void Vec2<T>::set_x(T x) noexcept {
  mX = x;
}

template<typename T>
void Vec2<T>::set_y(T y) noexcept {
  mY = y;
}

template<typename T>
constexpr auto Vec2<T>::get_x() const noexcept -> T {
  return mX;
}

template<typename T>
constexpr auto Vec2<T>::get_y() const noexcept -> T {
  return mY;
}

using Vec2f32 = Vec2<f32>;
using Vec2i32 = Vec2<i32>;

} // namespace basalt::math

#endif // !BS_MATH_VEC2_H
