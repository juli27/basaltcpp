#include "vector.h"
#include "vector2.h"
#include "vector3.h"
#include "vector4.h"

#include "angle.h"

#include <cmath>

namespace basalt {

template <typename Derived, typename T, uSize Size>
auto detail::Vector<Derived, T, Size>::length() const noexcept -> T {
  return std::sqrt(length_squared());
}

template <typename Derived, typename T, uSize Size>
auto detail::Vector<Derived, T, Size>::normalize() const noexcept -> Derived {
  return *self() / length();
}

template <typename Derived, typename T, uSize Size>
auto detail::Vector<Derived, T, Size>::distance(Derived const& p1,
                                                Derived const& p2) noexcept
  -> T {
  return (p2 - p1).length();
}

template auto detail::Vector<Vector2f32, f32, 2>::length() const noexcept
  -> f32;
template auto detail::Vector<Vector2f32, f32, 2>::normalize() const noexcept
  -> Vector2f32;
template auto
detail::Vector<Vector2f32, f32, 2>::distance(Vector2f32 const&,
                                             Vector2f32 const&) noexcept -> f32;

auto Vector2f32::normalized(f32 const x, f32 const y) noexcept -> Vector2f32 {
  return Vector2f32{x, y}.normalize();
}

// TODO: tbVector3Min, tbVector3Max
// TODO: tbVector3Random
// TODO: tbVector3InterpolateCoords, tbVector3InterpolateNormal

template auto detail::Vector<Vector3f32, f32, 3>::length() const noexcept
  -> f32;
template auto detail::Vector<Vector3f32, f32, 3>::normalize() const noexcept
  -> Vector3f32;
template auto
detail::Vector<Vector3f32, f32, 3>::distance(Vector3f32 const&,
                                             Vector3f32 const&) noexcept -> f32;

auto Vector3f32::normalized(f32 const x, f32 const y, f32 const z) noexcept
  -> Vector3f32 {
  return Vector3f32{x, y, z}.normalize();
}

auto Vector3f32::angle(Vector3f32 const& v1, Vector3f32 const& v2) -> Angle {
  return Angle::arccos(v1.dot(v2) /
                       std::sqrt(v1.length_squared() * v2.length_squared()));
}

template auto detail::Vector<Vector4f32, f32, 4>::length() const noexcept
  -> f32;
template auto detail::Vector<Vector4f32, f32, 4>::normalize() const noexcept
  -> Vector4f32;
template auto
detail::Vector<Vector4f32, f32, 4>::distance(Vector4f32 const&,
                                             Vector4f32 const&) noexcept -> f32;

auto Vector4f32::normalized(f32 const x, f32 const y, f32 const z,
                           f32 const w) noexcept -> Vector4f32 {
  return Vector4f32{x, y, z, w}.normalize();
}

} // namespace basalt
