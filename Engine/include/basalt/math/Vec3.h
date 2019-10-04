#pragma once
#ifndef BS_MATH_VEC3_H
#define BS_MATH_VEC3_H

#include <basalt/common/Types.h>

#include <cmath>

namespace basalt::math {

template <typename T>
struct Vec3 final {
  constexpr Vec3() noexcept = default;
  constexpr Vec3(T x, T y, T z) noexcept;
  constexpr Vec3(const Vec3&) noexcept = default;
  constexpr Vec3(Vec3&&) noexcept = default;
  ~Vec3() noexcept = default;

  auto operator=(const Vec3&) noexcept -> Vec3& = default;
  auto operator=(Vec3&&) noexcept -> Vec3& = default;

  auto operator-() const noexcept -> Vec3;
  auto operator+=(const Vec3& rhs) noexcept -> Vec3&;

  [[nodiscard]] auto length() const -> T;

  void set(T x, T y, T z) noexcept;

  void set_x(T x) noexcept;
  void set_y(T y) noexcept;
  void set_z(T z) noexcept;

  [[nodiscard]] constexpr auto get_x() const noexcept -> T;
  [[nodiscard]] constexpr auto get_y() const noexcept -> T;
  [[nodiscard]] constexpr auto get_z() const noexcept -> T;

  static auto normalize(const Vec3& v) -> Vec3;
  static constexpr auto cross(const Vec3& lhs, const Vec3& rhs) -> Vec3;

private:
  T mX = {};
  T mY = {};
  T mZ = {};
};

template<typename T>
constexpr Vec3<T>::Vec3(T x, T y, T z) noexcept : mX{x}, mY{y}, mZ{z} {}

template <typename T>
auto Vec3<T>::operator-() const noexcept -> Vec3 {
  return Vec3(-mX, -mY, -mZ);
}

template <typename T>
auto Vec3<T>::operator+=(const Vec3& rhs) noexcept -> Vec3& {
  mX += rhs.mX;
  mY += rhs.mY;
  mZ += rhs.mZ;

  return *this;
}

template<typename T>
auto Vec3<T>::length() const -> T {
  // TODO: specialize/guard this function call
  return std::sqrt(mX * mX + mY * mY + mZ * mZ);
}

template<typename T>
void Vec3<T>::set(T x, T y, T z) noexcept {
  mX = x;
  mY = y;
  mZ = z;
}

template<typename T>
void Vec3<T>::set_x(T x) noexcept {
  mX = x;
}

template<typename T>
void Vec3<T>::set_y(T y) noexcept {
  mY = y;
}

template<typename T>
void Vec3<T>::set_z(T z) noexcept {
  mZ = z;
}

template<typename T>
constexpr auto Vec3<T>::get_x() const noexcept -> T {
  return mX;
}

template<typename T>
constexpr auto Vec3<T>::get_y() const noexcept -> T {
  return mY;
}

template<typename T>
constexpr auto Vec3<T>::get_z() const noexcept -> T {
  return mZ;
}

template<typename T>
auto Vec3<T>::normalize(const Vec3& v) -> Vec3 {
  return v / v.length();
}

template<typename T>
constexpr auto Vec3<T>::cross(const Vec3& lhs, const Vec3& rhs) -> Vec3 {
  return Vec3(
    lhs.mY * rhs.mZ - lhs.mZ * rhs.mY,
    lhs.mZ * rhs.mX - lhs.mX * rhs.mZ,
    lhs.mX * rhs.mY - lhs.mY * rhs.mX
  );
}

template<typename T>
auto operator-(const Vec3<T>& lhs, const Vec3<T>& rhs) -> Vec3<T> {
  return Vec3<T>(lhs.get_x() - rhs.get_x(), lhs.get_y() - rhs.get_y(), lhs.get_z() - rhs.get_z());
}

template<typename T>
auto operator*(const Vec3<T>& v, T f) noexcept -> Vec3<T> {
  return Vec3<T>(v.get_x() * f, v.get_y() * f, v.get_z() * f);
}

template<typename T>
auto operator/(const Vec3<T>& v, T f) -> Vec3<T> {
  return Vec3<T>(v.get_x() / f, v.get_y() / f, v.get_z() / f);
}

using Vec3f32 = Vec3<f32>;

} // namespace basalt::math

#endif // !BS_MATH_VEC3_H
