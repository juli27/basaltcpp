#pragma once
#ifndef BS_MATH_VEC3_H
#define BS_MATH_VEC3_H

#include <cmath>

#include <basalt/common/Types.h>

namespace basalt::math {


template <typename T>
class Vec3 final {
public:
  constexpr Vec3();
  constexpr Vec3(T x, T y, T z);
  constexpr Vec3(const Vec3<T>&) = default;
  constexpr Vec3(Vec3<T>&&) = default;
  inline ~Vec3() = default;

  inline auto Length() const -> T;

  inline void Set(T x, T y, T z);
  inline void SetX(T x);
  inline void SetY(T y);
  inline void SetZ(T z);

  constexpr auto GetX() const -> T;
  constexpr auto GetY() const -> T;
  constexpr auto GetZ() const -> T;

  inline Vec3<T>& operator=(const Vec3<T>&) = default;
  inline Vec3<T>& operator=(Vec3<T>&&) = default;

private:
  T mX;
  T mY;
  T mZ;

public:
  static inline auto Normalize(const Vec3<T>& v) -> Vec3<T>;

  static constexpr auto Cross(const Vec3<T>& v1, const Vec3<T>& v2) -> Vec3<T>;

  template<typename V>
  friend auto operator-(const Vec3<V>& v) -> Vec3<V>;

  template<typename V>
  friend auto operator-(const Vec3<V>& a, const Vec3<V>& b) -> Vec3<V>;

  template<typename V>
  friend auto operator/(const Vec3<V>& a, V b) -> Vec3<V>;
};


template <typename T>
constexpr Vec3<T>::Vec3() : mX{}, mY{}, mZ{} {}


template<typename T>
constexpr Vec3<T>::Vec3(T x, T y, T z) : mX{x}, mY{y}, mZ{z} {}


template<typename T>
inline auto Vec3<T>::Length() const -> T {
  // TODO: specialize/guard this function call
  return std::sqrt(mX * mX + mY * mY + mZ * mZ);
}


template<typename T>
inline void Vec3<T>::Set(T x, T y, T z) {
  mX = x;
  mY = y;
  mZ = z;
}

template<typename T>
inline void Vec3<T>::SetX(T x) {
  mX = x;
}

template<typename T>
inline void Vec3<T>::SetY(T y) {
  mY = y;
}

template<typename T>
inline void Vec3<T>::SetZ(T z) {
  mZ = z;
}

template<typename T>
constexpr auto Vec3<T>::GetX() const -> T {
  return mX;
}

template<typename T>
constexpr auto Vec3<T>::GetY() const -> T {
  return mY;
}

template<typename T>
constexpr auto Vec3<T>::GetZ() const -> T {
  return mZ;
}


template<typename T>
inline auto Vec3<T>::Normalize(const Vec3<T>& v) -> Vec3<T> {
  return v / v.Length();
}


template<typename T>
constexpr auto Vec3<T>::Cross(const Vec3<T>& v1, const Vec3<T>& v2) -> Vec3<T> {
  return Vec3<T>(
    v1.mY * v2.mZ - v1.mZ * v2.mY,
    v1.mZ * v2.mX - v1.mX * v2.mZ,
    v1.mX * v2.mY - v1.mY * v2.mX
  );
}


template<typename T>
inline auto operator-(const Vec3<T>& v) -> Vec3<T> {
  return Vec3<T>(-v.mX, -v.mY, -v.mZ);
}


template<typename T>
inline auto operator-(const Vec3<T>& a, const Vec3<T>& b) -> Vec3<T> {
  return Vec3<T>(a.mX - b.mX, a.mY - b.mY, a.mZ - b.mZ);
}


template<typename T>
inline auto operator/(const Vec3<T>& v, T f) -> Vec3<T> {
  return Vec3<T>(v.mX / f, v.mY / f, v.mZ / f);
}


using Vec3f32 = Vec3<f32>;
using Vec3f64 = Vec3<f64>;

using Vec3i32 = Vec3<i32>;
using Vec3i64 = Vec3<i64>;

using Vec3u32 = Vec3<u32>;
using Vec3u64 = Vec3<u64>;

} // namespace basalt::math

#endif // !BS_MATH_VEC3_H
