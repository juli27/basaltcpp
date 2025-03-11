#pragma once

#include "vector.h"

#include "types.h"

#include "basalt/api/base/types.h"

#include <array>

namespace basalt {

class Vector4f32 : public detail::Vector<Vector4f32, f32, 4> {
public:
  [[nodiscard]]
  static auto normalized(f32 x, f32 y, f32 z, f32 w) noexcept -> Vector4f32;

  // sets every component to 0
  constexpr Vector4f32() noexcept = default;

  // sets every component to v
  constexpr explicit Vector4f32(f32 const v) noexcept : Vector{v, v, v, v} {
  }

  constexpr Vector4f32(f32 const x, f32 const y, f32 const z,
                       f32 const w) noexcept
    : Vector{x, y, z, w} {
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
  constexpr auto w() const noexcept -> f32 {
    return std::get<3>(components);
  }

  [[nodiscard]]
  constexpr auto w() noexcept -> f32& {
    return std::get<3>(components);
  }
};

extern template auto detail::Vector<Vector4f32, f32, 4>::length() const noexcept
  -> f32;
extern template auto
detail::Vector<Vector4f32, f32, 4>::normalize() const noexcept -> Vector4f32;
extern template auto
detail::Vector<Vector4f32, f32, 4>::distance(Vector4f32 const&,
                                             Vector4f32 const&) noexcept -> f32;

} // namespace basalt
