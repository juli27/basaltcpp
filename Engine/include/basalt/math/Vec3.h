#pragma once
#ifndef BS_MATH_VEC3_H
#define BS_MATH_VEC3_H

#include <cmath>

#include <basalt/common/Types.h>

namespace basalt::math {


template <typename T>
class Vec3 final {
public:
  constexpr Vec3() noexcept = default;
  constexpr Vec3(T x, T y, T z) noexcept;
  constexpr Vec3(const Vec3&) noexcept = default;
  constexpr Vec3(Vec3&&) noexcept = default;
  inline ~Vec3() noexcept = default;

  inline auto operator=(const Vec3&) noexcept -> Vec3& = default;
  inline auto operator=(Vec3&&) noexcept -> Vec3& = default;

  inline auto operator-() const noexcept -> Vec3;

  [[nodiscard]] inline auto Length() const -> T;

  inline void Set(T x, T y, T z) noexcept;

  inline void SetX(T x) noexcept;
  inline void SetY(T y) noexcept;
  inline void SetZ(T z) noexcept;

  [[nodiscard]] constexpr auto GetX() const noexcept -> T;
  [[nodiscard]] constexpr auto GetY() const noexcept -> T;
  [[nodiscard]] constexpr auto GetZ() const noexcept -> T;

private:
  T mX = {};
  T mY = {};
  T mZ = {};

public:
  static inline auto Normalize(const Vec3& v) -> Vec3;
  static constexpr auto Cross(const Vec3& lhs, const Vec3& rhs) -> Vec3;
};


template<typename T>
constexpr Vec3<T>::Vec3(T x, T y, T z) noexcept : mX{x}, mY{y}, mZ{z} {}


template <typename T>
inline auto Vec3<T>::operator-() const noexcept -> Vec3 {
  return Vec3(-mX, -mY, -mZ);
}


template<typename T>
inline auto Vec3<T>::Length() const -> T {
  // TODO: specialize/guard this function call
  return std::sqrt(mX * mX + mY * mY + mZ * mZ);
}


template<typename T>
inline void Vec3<T>::Set(T x, T y, T z) noexcept {
  mX = x;
  mY = y;
  mZ = z;
}


template<typename T>
inline void Vec3<T>::SetX(T x) noexcept {
  mX = x;
}


template<typename T>
inline void Vec3<T>::SetY(T y) noexcept {
  mY = y;
}

template<typename T>
inline void Vec3<T>::SetZ(T z) noexcept {
  mZ = z;
}


template<typename T>
constexpr auto Vec3<T>::GetX() const noexcept -> T {
  return mX;
}


template<typename T>
constexpr auto Vec3<T>::GetY() const noexcept -> T {
  return mY;
}


template<typename T>
constexpr auto Vec3<T>::GetZ() const noexcept -> T {
  return mZ;
}


template<typename T>
inline auto Vec3<T>::Normalize(const Vec3& v) -> Vec3 {
  return v / v.Length();
}


template<typename T>
constexpr auto Vec3<T>::Cross(const Vec3& lhs, const Vec3& rhs) -> Vec3 {
  return Vec3(
    lhs.mY * rhs.mZ - lhs.mZ * rhs.mY,
    lhs.mZ * rhs.mX - lhs.mX * rhs.mZ,
    lhs.mX * rhs.mY - lhs.mY * rhs.mX
  );
}


template<typename T>
inline auto operator-(const Vec3<T>& lhs, const Vec3<T>& rhs) -> Vec3<T> {
  return Vec3<T>(lhs.GetX() - rhs.GetX(), lhs.GetY() - rhs.GetY(), lhs.GetZ() - rhs.GetZ());
}


template<typename T>
inline auto operator/(const Vec3<T>& v, T f) -> Vec3<T> {
  return Vec3<T>(v.GetX() / f, v.GetY() / f, v.GetZ() / f);
}


using Vec3f32 = Vec3<f32>;
using Vec3f64 = Vec3<f64>;

using Vec3i32 = Vec3<i32>;
using Vec3i64 = Vec3<i64>;

using Vec3u32 = Vec3<u32>;
using Vec3u64 = Vec3<u64>;

} // namespace basalt::math

#endif // !BS_MATH_VEC3_H
