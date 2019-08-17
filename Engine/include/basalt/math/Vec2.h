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
  inline ~Vec2() noexcept = default;

  inline auto operator=(const Vec2&) noexcept -> Vec2& = default;
  inline auto operator=(Vec2&&) noexcept -> Vec2& = default;

  inline void Set(T x, T y) noexcept;

  inline void SetX(T x) noexcept;
  inline void SetY(T y) noexcept;

  constexpr auto GetX() const noexcept -> T;
  constexpr auto GetY() const noexcept -> T;

private:
  T mX = {};
  T mY = {};
};


template<typename T>
constexpr Vec2<T>::Vec2(const T x, const T y) noexcept : mX(x), mY(y) {}


template<typename T>
inline void Vec2<T>::Set(T x, T y) noexcept {
  mX = x;
  mY = y;
}


template<typename T>
inline void Vec2<T>::SetX(T x) noexcept {
  mX = x;
}


template<typename T>
inline void Vec2<T>::SetY(T y) noexcept {
  mY = y;
}


template<typename T>
constexpr auto Vec2<T>::GetX() const noexcept -> T {
  return mX;
}


template<typename T>
constexpr auto Vec2<T>::GetY() const noexcept -> T {
  return mY;
}


using Vec2f32 = Vec2<f32>;

using Vec2i32 = Vec2<i32>;

} // namespace basalt::math

#endif // !BS_MATH_VEC2_H
