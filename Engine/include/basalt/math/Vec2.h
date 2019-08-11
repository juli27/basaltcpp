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

  constexpr T GetX() const;
  constexpr T GetY() const;

  inline Vec2<T>& operator=(const Vec2<T>&) = default;
  inline Vec2<T>& operator=(Vec2<T>&&) = default;

private:
  T m_x;
  T m_y;
};

template <typename T>
constexpr Vec2<T>::Vec2() : m_x{}, m_y{} {}

template<typename T>
constexpr Vec2<T>::Vec2(T x, T y) : m_x(x), m_y(y) {}

template<typename T>
inline void Vec2<T>::Set(T x, T y) {
  m_x = x;
  m_y = y;
}

template<typename T>
inline void Vec2<T>::SetX(T x) {
  m_x = x;
}

template<typename T>
inline void Vec2<T>::SetY(T y) {
  m_y = y;
}

template<typename T>
constexpr T Vec2<T>::GetX() const {
  return m_x;
}

template<typename T>
constexpr T Vec2<T>::GetY() const {
  return m_y;
}


using Vec2f32 = Vec2<f32>;

using Vec2i32 = Vec2<i32>;

} // namespace basalt::math

#endif // !BS_MATH_VEC2_H
