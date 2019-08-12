#pragma once
#ifndef BS_MATH_VEC2_H
#define BS_MATH_VEC2_H

#include <basalt/common/Types.h>

namespace basalt::math {


template <typename T>
class Vec2 final {
public:
  constexpr Vec2();
  constexpr Vec2(T x, T y);
  constexpr Vec2(const Vec2<T>&) = default;
  constexpr Vec2(Vec2<T>&&) = default;
  inline ~Vec2() = default;

  inline void Set(T x, T y);
  inline void SetX(T x);
  inline void SetY(T y);

  constexpr auto GetX() const -> T;
  constexpr auto GetY() const -> T;

  inline auto operator=(const Vec2<T>&) -> Vec2<T>& = default;
  inline auto operator=(Vec2<T>&&) -> Vec2<T>& = default;

private:
  T mX;
  T mY;
};

template <typename T>
constexpr Vec2<T>::Vec2() : mX{}, mY{} {}

template<typename T>
constexpr Vec2<T>::Vec2(T x, T y) : mX(x), mY(y) {}

template<typename T>
inline void Vec2<T>::Set(T x, T y) {
  mX = x;
  mY = y;
}

template<typename T>
inline void Vec2<T>::SetX(T x) {
  mX = x;
}

template<typename T>
inline void Vec2<T>::SetY(T y) {
  mY = y;
}

template<typename T>
constexpr auto Vec2<T>::GetX() const -> T {
  return mX;
}

template<typename T>
constexpr auto Vec2<T>::GetY() const -> T {
  return mY;
}


using Vec2f32 = Vec2<f32>;

using Vec2i32 = Vec2<i32>;

} // namespace basalt::math

#endif // !BS_MATH_VEC2_H
