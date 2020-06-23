#include "vec3.h"

#include <cmath>

namespace basalt {

template <typename T>
auto Vec3<T>::operator+=(const Vec3& v) noexcept -> Vec3& {
  x += v.x;
  y += v.y;
  z += v.z;

  return *this;
}

template <typename T>
auto Vec3<T>::operator-=(const Vec3& v) noexcept -> Vec3& {
  x -= v.x;
  y -= v.y;
  z -= v.z;

  return *this;
}

template <typename T>
auto Vec3<T>::operator*=(const T s) noexcept -> Vec3& {
  x *= s;
  y *= s;
  z *= s;

  return *this;
}

template <typename T>
auto Vec3<T>::operator/=(const T s) noexcept -> Vec3& {
  x /= s;
  y /= s;
  z /= s;

  return *this;
}

template <typename T>
auto Vec3<T>::operator==(const Vec3& o) const noexcept -> bool {
  return x == o.x && y == o.y && z == o.z;
}

template <typename T>
auto Vec3<T>::operator!=(const Vec3& o) const noexcept -> bool {
  return !(*this == o);
}

template <typename T>
auto Vec3<T>::length_squared() const noexcept -> T {
  return x * x + y * y + z * z;
}

template <typename T>
auto Vec3<T>::length() const -> T {
  return std::sqrt(length_squared());
}

template <typename T>
void Vec3<T>::set(const T ax, const T ay, const T az) noexcept {
  x = ax;
  y = ay;
  z = az;
}

template <typename T>
auto Vec3<T>::normalize(const Vec3& v) -> Vec3 {
  return v / v.length();
}

template <typename T>
auto Vec3<T>::dot(const Vec3& l, const Vec3& r) noexcept -> T {
  return l.x * r.x + l.y * r.y + l.z * r.z;
}

template struct Vec3<f32>;


template <typename T>
auto operator+(const Vec3<T>& l, const Vec3<T>& r) noexcept -> Vec3<T> {
  return Vec3<T> {l.x + r.x, l.y + r.y, l.z + r.z};
}

template auto operator+(const Vec3<f32>&, const Vec3<f32>&) -> Vec3<f32>;


template <typename T>
auto operator-(const Vec3<T>& l, const Vec3<T>& r) noexcept -> Vec3<T> {
  return Vec3<T> {l.x - r.x, l.y - r.y, l.z - r.z};
}

template auto operator-(const Vec3<f32>&, const Vec3<f32>&) -> Vec3<f32>;


template <typename T>
auto operator*(const Vec3<T>& v, const T s) noexcept -> Vec3<T> {
  return Vec3<T> {v.x * s, v.y * s, v.z * s};
}

template auto operator*(const Vec3<f32>&, f32) noexcept -> Vec3<f32>;


template <typename T>
auto operator*(const T s, const Vec3<T>& v) noexcept -> Vec3<T> {
  return v * s;
}

template auto operator*(f32, const Vec3<f32>&) noexcept -> Vec3<f32>;


template <typename T>
auto operator/(const Vec3<T>& v, const T s) noexcept -> Vec3<T> {
  return Vec3<T> {v.x / s, v.y / s, v.z / s};
}

template auto operator/(const Vec3<f32>&, f32) -> Vec3<f32>;

} // namespace basalt
