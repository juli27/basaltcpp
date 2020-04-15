#include "runtime/math/Vec3.h"

#include <cmath>

namespace basalt::math {

template struct Vec3<f32>;

template <typename T>
auto Vec3<T>::operator-() const noexcept -> Vec3 {
  return Vec3(-x(), -y(), -z());
}

template auto Vec3<f32>::operator-() const noexcept -> Vec3;

template <typename T>
auto Vec3<T>::operator+=(const Vec3& rhs) noexcept -> Vec3& {
  std::get<0>(mData) += rhs.x();
  std::get<1>(mData) += rhs.y();
  std::get<2>(mData) += rhs.z();

  return *this;
}

template auto Vec3<f32>::operator+=(const Vec3& rhs) noexcept -> Vec3&;

template<typename T>
auto Vec3<T>::length() const -> T {
  // TODO: specialize/guard this function call
  return std::sqrt(x() * x() + y() * y() + z() * z());
}

template auto Vec3<f32>::length() const->f32;

template<typename T>
void Vec3<T>::set(T x, T y, T z) noexcept {
  set_x(x);
  set_y(y);
  set_z(z);
}

template void Vec3<f32>::set(f32 x, f32 y, f32 z) noexcept;

template<typename T>
void Vec3<T>::set_x(T x) noexcept {
  std::get<0>(mData) = x;
}

template void Vec3<f32>::set_x(f32 x) noexcept;

template<typename T>
void Vec3<T>::set_y(T y) noexcept {
  std::get<1>(mData) = y;
}

template void Vec3<f32>::set_y(f32 y) noexcept;

template<typename T>
void Vec3<T>::set_z(T z) noexcept {
  std::get<2>(mData) = z;
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
