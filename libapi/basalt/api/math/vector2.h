#pragma once

#include "vector.h"

#include "types.h"

#include "basalt/api/base/types.h"

#include <array>

namespace basalt {

class Vector2f32 : public detail::Vector<Vector2f32, f32, 2> {
public:
  [[nodiscard]]
  static auto normalized(f32 x, f32 y) noexcept -> Vector2f32;

  // sets every component to 0
  constexpr Vector2f32() noexcept = default;

  // sets every component to v
  constexpr explicit Vector2f32(f32 const v) noexcept : Vector{v, v} {
  }

  constexpr Vector2f32(f32 const x, f32 const y) noexcept : Vector{x, y} {
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
};

extern template auto detail::Vector<Vector2f32, f32, 2>::length() const noexcept
  -> f32;
extern template auto
detail::Vector<Vector2f32, f32, 2>::normalize() const noexcept -> Vector2f32;
extern template auto
detail::Vector<Vector2f32, f32, 2>::distance(Vector2f32 const&,
                                             Vector2f32 const&) noexcept -> f32;

} // namespace basalt
