#include <basalt/api/math/vector3.h>

#include <basalt/api/math/angle.h>

#include <cmath>

namespace basalt {

// TODO: tbVector3Min, tbVector3Max
// TODO: tbVector3Random
// TODO: tbVector3InterpolateCoords, tbVector3InterpolateNormal

auto Vector3f32::length() const noexcept -> f32 {
  return std::sqrt(length_squared()); // TODO: std::hypot() ?
}

auto Vector3f32::normalize(const Vector3f32& v) -> Vector3f32 {
  return v / v.length();
}

auto Vector3f32::angle(const Vector3f32& v1, const Vector3f32& v2) -> Angle {
  return Angle::arccos(dot(v1, v2) /
                       std::sqrt(v1.length_squared() * v2.length_squared()));
}

} // namespace basalt
