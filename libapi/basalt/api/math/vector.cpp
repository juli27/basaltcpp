#include "vector_p.h"
#include "vector2.h"
#include "vector3.h"
#include "vector4.h"

#include <cmath>

namespace basalt {
template <typename Derived, typename T, uSize Dimension>
auto detail::Vector<Derived, T, Dimension>::normalized(Derived nonZeroVec)
  -> Derived {
  nonZeroVec.normalize();
  return nonZeroVec;
}

template <typename Derived, typename T, uSize Dimension>
auto detail::Vector<Derived, T, Dimension>::normalize() -> Derived& {
  *this /= length();
  return self();
}

template <typename Derived, typename T, uSize Dimension>
auto detail::Vector<Derived, T, Dimension>::angle_to(Derived const& other) const
  -> Angle {
  return Angle::arccos(dot(other) /
                       std::sqrt(length_squared() * other.length_squared()));
}

template <typename Derived, typename T, uSize Dimension>
auto detail::Vector<Derived, T, Dimension>::distance_to(
  Derived const& other) const -> T {
  return (other - self()).length();
}

template <typename Derived, typename T, uSize Dimension>
auto detail::Vector<Derived, T, Dimension>::length() const -> T {
  return std::sqrt(length_squared());
}

template auto detail::Vector<Vector2f32, f32, 2>::normalized(Vector2f32)
  -> Vector2f32;
template auto detail::Vector<Vector2f32, f32, 2>::normalize() -> Vector2f32&;
template auto
detail::Vector<Vector2f32, f32, 2>::angle_to(Vector2f32 const&) const -> Angle;
template auto
detail::Vector<Vector2f32, f32, 2>::distance_to(Vector2f32 const&) const -> f32;
template auto detail::Vector<Vector2f32, f32, 2>::length() const -> f32;

auto Vector2f32::normalized(f32 const x, f32 const y) -> Vector2f32 {
  return normalized(Vector2f32{x, y});
}

// TODO: tbVector3Min, tbVector3Max
// TODO: tbVector3Random
// TODO: tbVector3InterpolateCoords, tbVector3InterpolateNormal

template auto detail::Vector<Vector3f32, f32, 3>::normalized(Vector3f32)
  -> Vector3f32;
template auto detail::Vector<Vector3f32, f32, 3>::normalize() -> Vector3f32&;
template auto
detail::Vector<Vector3f32, f32, 3>::angle_to(Vector3f32 const&) const -> Angle;
template auto
detail::Vector<Vector3f32, f32, 3>::distance_to(Vector3f32 const&) const -> f32;
template auto detail::Vector<Vector3f32, f32, 3>::length() const -> f32;

auto Vector3f32::normalized(f32 const x, f32 const y, f32 const z)
  -> Vector3f32 {
  return normalized(Vector3f32{x, y, z});
}

template auto detail::Vector<Vector4f32, f32, 4>::normalized(Vector4f32)
  -> Vector4f32;
template auto detail::Vector<Vector4f32, f32, 4>::normalize() -> Vector4f32&;
template auto
detail::Vector<Vector4f32, f32, 4>::angle_to(Vector4f32 const&) const -> Angle;
template auto
detail::Vector<Vector4f32, f32, 4>::distance_to(Vector4f32 const&) const -> f32;
template auto detail::Vector<Vector4f32, f32, 4>::length() const -> f32;

auto Vector4f32::normalized(f32 const x, f32 const y, f32 const z, f32 const w)
  -> Vector4f32 {
  return normalized(Vector4f32{x, y, z, w});
}

} // namespace basalt
