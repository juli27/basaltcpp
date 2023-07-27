#include <basalt/api/math/vector.h>
#include <basalt/api/math/vector2.h>
#include <basalt/api/math/vector3.h>
#include <basalt/api/math/vector4.h>

#include <basalt/api/math/angle.h>

#include <cmath>

namespace basalt {

template <typename Derived, typename T, uSize Size>
auto detail::Vector<Derived, T, Size>::normalize(Derived v) noexcept
  -> Derived {
  v.normalize();

  return v;
}

template <typename Derived, typename T, uSize Size>
auto detail::Vector<Derived, T, Size>::length() const noexcept -> T {
  return std::sqrt(length_squared());
}

template <typename Derived, typename T, uSize Size>
auto detail::Vector<Derived, T, Size>::normalize() const noexcept -> Derived {
  return *self() / length();
}

template <typename Derived, typename T, uSize Size>
auto detail::Vector<Derived, T, Size>::distance(const Derived& p1,
                                                const Derived& p2) noexcept
  -> T {
  return (p2 - p1).length();
}

template auto detail::Vector<Vector2f32, f32, 2>::length() const noexcept
  -> f32;
template auto detail::Vector<Vector2f32, f32, 2>::normalize() const noexcept
  -> Vector2f32;
template auto detail::Vector<Vector2f32, f32, 2>::normalize(Vector2f32) noexcept
  -> Vector2f32;
template auto
detail::Vector<Vector2f32, f32, 2>::distance(const Vector2f32&,
                                             const Vector2f32&) noexcept -> f32;

auto Vector2f32::normalize(const f32 x, const f32 y) noexcept -> Vector2f32 {
  return normalize(Vector2f32 {x, y});
}

// TODO: tbVector3Min, tbVector3Max
// TODO: tbVector3Random
// TODO: tbVector3InterpolateCoords, tbVector3InterpolateNormal

template auto detail::Vector<Vector3f32, f32, 3>::length() const noexcept
  -> f32;
template auto detail::Vector<Vector3f32, f32, 3>::normalize() const noexcept
  -> Vector3f32;
template auto detail::Vector<Vector3f32, f32, 3>::normalize(Vector3f32) noexcept
  -> Vector3f32;
template auto
detail::Vector<Vector3f32, f32, 3>::distance(const Vector3f32&,
                                             const Vector3f32&) noexcept -> f32;

auto Vector3f32::normalize(const f32 x, const f32 y, const f32 z) noexcept
  -> Vector3f32 {
  return normalize(Vector3f32 {x, y, z});
}

auto Vector3f32::angle(const Vector3f32& v1, const Vector3f32& v2) -> Angle {
  return Angle::arccos(v1.dot(v2) /
                       std::sqrt(v1.length_squared() * v2.length_squared()));
}

template auto detail::Vector<Vector4f32, f32, 4>::length() const noexcept
  -> f32;
template auto detail::Vector<Vector4f32, f32, 4>::normalize() const noexcept
  -> Vector4f32;
template auto detail::Vector<Vector4f32, f32, 4>::normalize(Vector4f32) noexcept
  -> Vector4f32;
template auto
detail::Vector<Vector4f32, f32, 4>::distance(const Vector4f32&,
                                             const Vector4f32&) noexcept -> f32;

auto Vector4f32::normalize(const f32 x, const f32 y, const f32 z,
                           const f32 w) noexcept -> Vector4f32 {
  return normalize(Vector4f32 {x, y, z, w});
}

} // namespace basalt
