#pragma once
#ifndef BS_MATH_VEC3_H
#define BS_MATH_VEC3_H

#include <cmath>

#include <basalt/common/Types.h>

namespace basalt {
namespace math {

template <typename T>
class Vec3 final {
public:
  constexpr Vec3();
  constexpr Vec3(T x, T y, T z);
  constexpr Vec3(const Vec3<T>&) = default;
  constexpr Vec3(Vec3<T>&&) = default;
  inline ~Vec3() = default;

  inline T Length() const;

  inline void Set(T x, T y, T z);
  inline void SetX(T x);
  inline void SetY(T y);
  inline void SetZ(T z);

  constexpr T GetX() const;
  constexpr T GetY() const;
  constexpr T GetZ() const;

  inline Vec3<T>& operator=(const Vec3<T>&) = default;
  inline Vec3<T>& operator=(Vec3<T>&&) = default;

private:
  T m_x;
  T m_y;
  T m_z;

public:
  static inline Vec3<T> Normalize(const Vec3<T>& v);

  static constexpr Vec3<T> Cross(const Vec3<T>& v1, const Vec3<T>& v2);

  template<typename V>
  friend Vec3<V> operator-(const Vec3<V>& v);

  template<typename V>
  friend Vec3<V> operator-(const Vec3<V>& a, const Vec3<V>& b);

  template<typename V>
  friend Vec3<V> operator/(const Vec3<V>& a, V b);
};


template <typename T>
constexpr Vec3<T>::Vec3() : m_x{}, m_y{}, m_z{} {}


template<typename T>
constexpr Vec3<T>::Vec3(T x, T y, T z) : m_x{x}, m_y{y}, m_z{z} {}


template<typename T>
inline T Vec3<T>::Length() const {
  // TODO: specialize/guard this function call
  return std::sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
}


template<typename T>
inline void Vec3<T>::Set(T x, T y, T z) {
  m_x = x;
  m_y = y;
  m_z = z;
}

template<typename T>
inline void Vec3<T>::SetX(T x) {
  m_x = x;
}

template<typename T>
inline void Vec3<T>::SetY(T y) {
  m_y = y;
}

template<typename T>
inline void Vec3<T>::SetZ(T z) {
  m_z = z;
}

template<typename T>
constexpr T Vec3<T>::GetX() const {
  return m_x;
}

template<typename T>
constexpr T Vec3<T>::GetY() const {
  return m_y;
}

template<typename T>
constexpr T Vec3<T>::GetZ() const {
  return m_z;
}


template<typename T>
inline Vec3<T> Vec3<T>::Normalize(const Vec3<T>& v) {
  return v / v.Length();
}


template<typename T>
constexpr Vec3<T> Vec3<T>::Cross(const Vec3<T>& v1, const Vec3<T>& v2) {
  return Vec3<T>(
    v1.m_y * v2.m_z - v1.m_z * v2.m_y,
    v1.m_z * v2.m_x - v1.m_x * v2.m_z,
    v1.m_x * v2.m_y - v1.m_y * v2.m_x
  );
}


template<typename T>
inline Vec3<T> operator-(const Vec3<T>& v) {
  return Vec3<T>(-v.m_x, -v.m_y, -v.m_z);
}


template<typename T>
inline Vec3<T> operator-(const Vec3<T>& a, const Vec3<T>& b) {
  return Vec3<T>(a.m_x - b.m_x, a.m_y - b.m_y, a.m_z - b.m_z);
}


template<typename T>
inline Vec3<T> operator/(const Vec3<T>& v, T f) {
  return Vec3<T>(v.m_x / f, v.m_y / f, v.m_z / f);
}


using Vec3f32 = Vec3<f32>;
using Vec3f64 = Vec3<f64>;

using Vec3i32 = Vec3<i32>;
using Vec3i64 = Vec3<i64>;

using Vec3u32 = Vec3<u32>;
using Vec3u64 = Vec3<u64>;

} // namespace math
} // namespace basalt

#endif // !BS_MATH_VEC3_H
