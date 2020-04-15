#include "runtime/math/Vec3.h"

#include "runtime/shared/Types.h"

#include <cmath>

namespace basalt::math {

template struct Vec3<f32>;

template <typename T>
auto Vec3<T>::operator-() const noexcept -> Vec3 {
  return Vec3(-mX, -mY, -mZ);
}

template auto Vec3<f32>::operator-() const noexcept -> Vec3;

template <typename T>
auto Vec3<T>::operator+=(const Vec3& rhs) noexcept -> Vec3& {
  mX += rhs.mX;
  mY += rhs.mY;
  mZ += rhs.mZ;

  return *this;
}

template auto Vec3<f32>::operator+=(const Vec3& rhs) noexcept -> Vec3&;

template<typename T>
auto Vec3<T>::length() const -> T {
  // TODO: specialize/guard this function call
  return std::sqrt(mX * mX + mY * mY + mZ * mZ);
}

template auto Vec3<f32>::length() const->f32;

template<typename T>
void Vec3<T>::set(T x, T y, T z) noexcept {
  mX = x;
  mY = y;
  mZ = z;
}

template void Vec3<f32>::set(f32 x, f32 y, f32 z) noexcept;

template<typename T>
void Vec3<T>::set_x(T x) noexcept {
  mX = x;
}

template void Vec3<f32>::set_x(f32 x) noexcept;

template<typename T>
void Vec3<T>::set_y(T y) noexcept {
  mY = y;
}

template void Vec3<f32>::set_y(f32 y) noexcept;

template<typename T>
void Vec3<T>::set_z(T z) noexcept {
  mZ = z;
}

template void Vec3<f32>::set_z(f32 z) noexcept;

template<typename T>
auto Vec3<T>::normalize(const Vec3& v) -> Vec3 {
  return v / v.length();
}

template auto Vec3<f32>::normalize(const Vec3& v) -> Vec3;

template<typename T>
auto operator-(const Vec3<T>& lhs, const Vec3<T>& rhs) -> Vec3<T> {
  return Vec3<T>(lhs.x() - rhs.x(), lhs.y() - rhs.y(), lhs.z() - rhs.z());
}

template auto operator-(const Vec3f32& lhs, const Vec3f32& rhs) -> Vec3f32;

template<typename T>
auto operator*(const Vec3<T>& v, T f) noexcept -> Vec3<T> {
  return Vec3<T>(v.x() * f, v.y() * f, v.z() * f);
}

template auto operator*(const Vec3f32& v, f32 f) noexcept -> Vec3f32;

template<typename T>
auto operator/(const Vec3<T>& v, T f) -> Vec3<T> {
  return Vec3<T>(v.x() / f, v.y() / f, v.z() / f);
}

template auto operator/(const Vec3f32& v, f32 f) -> Vec3f32;

} // namespace basalt::math
