#pragma once
#ifndef BASALT_MATH_VEC3_H
#define BASALT_MATH_VEC3_H

#include "runtime/shared/Types.h"

namespace basalt::math {

template <typename T>
struct Vec3 final {
  T x {};
  T y {};
  T z {};

  constexpr Vec3() noexcept = default;

  constexpr Vec3(const T x, const T y, const T z) noexcept
    : x {x}, y {y}, z {z} {
  }

  constexpr Vec3(const Vec3&) noexcept = default;
  constexpr Vec3(Vec3&&) noexcept = default;

  ~Vec3() noexcept = default;

  auto operator=(const Vec3&) noexcept -> Vec3& = default;
  auto operator=(Vec3&&) noexcept -> Vec3& = default;

  auto operator+=(const Vec3&) noexcept -> Vec3&;
  auto operator-=(const Vec3&) noexcept -> Vec3&;
  auto operator*=(T) noexcept -> Vec3&;
  auto operator/=(T) noexcept -> Vec3&;

  auto operator-() const noexcept -> Vec3;

  auto operator==(const Vec3&) const -> bool;
  auto operator!=(const Vec3&) const -> bool;

  [[nodiscard]]
  auto length_squared() const -> T;
  [[nodiscard]]
  auto length() const -> T;

  void set(T ax, T ay, T az) noexcept;

  [[nodiscard]]
  static auto normalize(const Vec3&) -> Vec3;

  [[nodiscard]]
  static constexpr auto cross(const Vec3& l, const Vec3& r) -> Vec3 {
    return Vec3 {
      l.y * r.z - l.z * r.y
    , l.z * r.x - l.x * r.z
    , l.x * r.y - l.y * r.x
    };
  }

  [[nodiscard]]
  static auto dot(const Vec3&, const Vec3&) -> T;
};

using Vec3f32 = Vec3<f32>;

extern template struct Vec3<f32>;
extern template auto Vec3<f32>::operator+=(const Vec3&) noexcept -> Vec3&;
extern template auto Vec3<f32>::operator-=(const Vec3&) noexcept -> Vec3&;
extern template auto Vec3<f32>::operator*=(f32) noexcept -> Vec3&;
extern template auto Vec3<f32>::operator/=(f32) noexcept -> Vec3&;
extern template auto Vec3<f32>::operator-() const noexcept -> Vec3;
extern template auto Vec3<f32>::operator==(const Vec3&) const -> bool;
extern template auto Vec3<f32>::operator!=(const Vec3&) const -> bool;
extern template auto Vec3<f32>::length_squared() const -> f32;
extern template auto Vec3<f32>::length() const -> f32;
extern template void Vec3<f32>::set(f32 ax, f32 ay, f32 az) noexcept;
extern template auto Vec3<f32>::normalize(const Vec3&) -> Vec3;
extern template auto Vec3<f32>::dot(const Vec3&, const Vec3&) -> f32;

template <typename T>
auto operator+(const Vec3<T>&, const Vec3<T>&) -> Vec3<T>;

extern template auto operator+(const Vec3<f32>&, const Vec3<f32>&) -> Vec3<f32>;

template <typename T>
auto operator-(const Vec3<T>&, const Vec3<T>&) -> Vec3<T>;

extern template auto operator-(const Vec3<f32>&, const Vec3<f32>&) -> Vec3<f32>;

template <typename T>
auto operator*(const Vec3<T>&, T) noexcept -> Vec3<T>;

extern template auto operator*(const Vec3<f32>&, f32) noexcept -> Vec3<f32>;

template <typename T>
auto operator*(T, const Vec3<T>&) noexcept -> Vec3<T>;

extern template auto operator*(f32, const Vec3<f32>&) noexcept -> Vec3<f32>;

template <typename T>
auto operator/(const Vec3<T>&, T) -> Vec3<T>;

extern template auto operator/(const Vec3<f32>&, f32) -> Vec3<f32>;

} // namespace basalt::math

#endif // BASALT_MATH_VEC3_H
