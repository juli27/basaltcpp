#pragma once
#ifndef BASALT_MATH_VEC3_H
#define BASALT_MATH_VEC3_H

#include "runtime/shared/Types.h"

namespace basalt::math {

template <typename T>
struct Vec3 final {
  constexpr Vec3() noexcept = default;
  constexpr Vec3(T x, T y, T z) noexcept : mX{ x }, mY{ y }, mZ{ z } {}
  constexpr Vec3(const Vec3&) noexcept = default;
  constexpr Vec3(Vec3&&) noexcept = default;
  ~Vec3() noexcept = default;

  auto operator=(const Vec3&) noexcept -> Vec3& = default;
  auto operator=(Vec3&&) noexcept -> Vec3& = default;

  auto operator-() const noexcept -> Vec3;
  auto operator+=(const Vec3& rhs) noexcept -> Vec3&;

  [[nodiscard]]
  auto length() const -> T;

  void set(T x, T y, T z) noexcept;

  void set_x(T x) noexcept;
  void set_y(T y) noexcept;
  void set_z(T z) noexcept;

  [[nodiscard]]
  constexpr auto x() const noexcept -> T {
    return mX;
  }

  [[nodiscard]]
  constexpr auto y() const noexcept -> T {
    return mY;
  }

  [[nodiscard]]
  constexpr auto z() const noexcept -> T {
    return mZ;
  }

  static auto normalize(const Vec3& v) -> Vec3;
  static constexpr auto cross(const Vec3& lhs, const Vec3& rhs) -> Vec3 {
    return Vec3(
      lhs.mY * rhs.mZ - lhs.mZ * rhs.mY,
      lhs.mZ * rhs.mX - lhs.mX * rhs.mZ,
      lhs.mX * rhs.mY - lhs.mY * rhs.mX
    );
  }

private:
  T mX = {};
  T mY = {};
  T mZ = {};
};

extern template struct Vec3<f32>;
extern template auto Vec3<f32>::operator-() const noexcept -> Vec3;
extern template auto Vec3<f32>::operator+=(const Vec3& rhs) noexcept -> Vec3&;
extern template auto Vec3<f32>::length() const -> f32;
extern template void Vec3<f32>::set(f32 x, f32 y, f32 z) noexcept;
extern template void Vec3<f32>::set_x(f32 x) noexcept;
extern template void Vec3<f32>::set_y(f32 y) noexcept;
extern template void Vec3<f32>::set_z(f32 z) noexcept;
extern template auto Vec3<f32>::normalize(const Vec3& v) -> Vec3;

using Vec3f32 = Vec3<f32>;

template<typename T>
auto operator-(const Vec3<T>& lhs, const Vec3<T>& rhs) -> Vec3<T>;

extern template auto operator-(const Vec3f32& lhs, const Vec3f32& rhs) -> Vec3f32;

template<typename T>
auto operator*(const Vec3<T>& v, T f) noexcept -> Vec3<T>;

extern template auto operator*(const Vec3f32& v, f32 f) noexcept -> Vec3f32;

template<typename T>
auto operator/(const Vec3<T>& v, T f) -> Vec3<T>;

extern template auto operator/(const Vec3f32& v, f32 f) -> Vec3f32;

} // namespace basalt::math

#endif // BASALT_MATH_VEC3_H
