#include "vector3.h"

#include <cmath>

namespace basalt {

auto Vector3f32::length_squared() const noexcept -> f32 {
  return x() * x() + y() * y() + z() * z();
}

auto Vector3f32::length() const noexcept -> f32 {
  return std::sqrt(length_squared());
}

void Vector3f32::set(const f32 ax, const f32 ay, const f32 az) noexcept {
  x() = ax;
  y() = ay;
  z() = az;
}

auto Vector3f32::normalize(const Vector3f32& v) noexcept -> Vector3f32 {
  return v / v.length();
}

auto Vector3f32::dot(const Vector3f32& l, const Vector3f32& r) noexcept -> f32 {
  return l.x() * r.x() + l.y() * r.y() + l.z() * r.z();
}

} // namespace basalt
