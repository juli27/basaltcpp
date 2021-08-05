#include <basalt/api/math/vector3.h>

#include <cmath>

namespace basalt {

auto Vector3f32::length() const noexcept -> f32 {
  return std::sqrt(length_squared());
}

auto Vector3f32::normalize(const Vector3f32& v) -> Vector3f32 {
  return v / v.length();
}

} // namespace basalt
