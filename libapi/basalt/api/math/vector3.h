#pragma once

#include "vector.h"

#include "types.h"

#include "basalt/api/base/types.h"

#include <array>

namespace basalt {

class Vector3f32 : public detail::Vector<Vector3f32, f32, 3> {
public:
  [[nodiscard]]
  static constexpr auto right() noexcept -> Vector3f32 {
    return Vector3f32{1.0f, 0.0f, 0.0f};
  }

  [[nodiscard]]
  static constexpr auto left() noexcept -> Vector3f32 {
    return Vector3f32{-1.0f, 0.0f, 0.0f};
  }

  [[nodiscard]]
  static constexpr auto up() noexcept -> Vector3f32 {
    return Vector3f32{0.0f, 1.0f, 0.0f};
  }

  [[nodiscard]]
  static constexpr auto down() noexcept -> Vector3f32 {
    return Vector3f32{0.0f, -1.0f, 0.0f};
  }

  [[nodiscard]]
  static constexpr auto forward() noexcept -> Vector3f32 {
    return Vector3f32{0.0f, 0.0f, 1.0f};
  }

  [[nodiscard]]
  static constexpr auto back() noexcept -> Vector3f32 {
    return Vector3f32{0.0f, 0.0f, -1.0f};
  }

  [[nodiscard]]
  static auto normalized(f32 x, f32 y, f32 z) noexcept -> Vector3f32;

  // sets every component to 0
  constexpr Vector3f32() noexcept = default;

  // sets every component to v
  constexpr explicit Vector3f32(f32 const v) noexcept : Vector{v, v, v} {
  }

  constexpr Vector3f32(f32 const x, f32 const y, f32 const z) noexcept
    : Vector{x, y, z} {
  }

  [[nodiscard]]
  constexpr auto x() const noexcept -> f32 {
    return std::get<0>(components);
  }

  [[nodiscard]]
  constexpr auto x() noexcept -> f32& {
    return std::get<0>(components);
  }

  [[nodiscard]]
  constexpr auto y() const noexcept -> f32 {
    return std::get<1>(components);
  }

  [[nodiscard]]
  constexpr auto y() noexcept -> f32& {
    return std::get<1>(components);
  }

  [[nodiscard]]
  constexpr auto z() const noexcept -> f32 {
    return std::get<2>(components);
  }

  [[nodiscard]]
  constexpr auto z() noexcept -> f32& {
    return std::get<2>(components);
  }

  [[nodiscard]]
  constexpr auto cross(Vector3f32 const& r) const noexcept -> Vector3f32 {
    return Vector3f32{
      y() * r.z() - z() * r.y(),
      z() * r.x() - x() * r.z(),
      x() * r.y() - y() * r.x(),
    };
  }

  [[nodiscard]]
  static constexpr auto cross(Vector3f32 const& l, Vector3f32 const& r) noexcept
    -> Vector3f32 {
    return l.cross(r);
  }

  [[nodiscard]]
  static auto angle(Vector3f32 const&, Vector3f32 const&) -> Angle;
};

extern template auto detail::Vector<Vector3f32, f32, 3>::length() const noexcept
  -> f32;
extern template auto
detail::Vector<Vector3f32, f32, 3>::normalize() const noexcept -> Vector3f32;
extern template auto
detail::Vector<Vector3f32, f32, 3>::distance(Vector3f32 const&,
                                             Vector3f32 const&) noexcept -> f32;

} // namespace basalt
