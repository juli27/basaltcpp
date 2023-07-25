#pragma once

#include <basalt/api/math/vector.h>

#include <basalt/api/math/types.h>

#include <basalt/api/base/types.h>

#include <array>

namespace basalt {

class Vector2f32 : public detail::Vector<Vector2f32, f32, 2> {
public:
  using Vector::normalize;
  [[nodiscard]] static auto normalize(f32 x, f32 y) noexcept -> Vector2f32;

  // sets every component to 0
  constexpr Vector2f32() noexcept = default;

  // sets every component to v
  constexpr explicit Vector2f32(const f32 v) noexcept : Vector {v, v} {
  }

  constexpr Vector2f32(const f32 x, const f32 y) noexcept : Vector {x, y} {
  }

  [[nodiscard]] constexpr auto x() const noexcept -> f32 {
    return std::get<0>(components);
  }

  [[nodiscard]] constexpr auto x() noexcept -> f32& {
    return std::get<0>(components);
  }

  [[nodiscard]] constexpr auto y() const noexcept -> f32 {
    return std::get<1>(components);
  }

  [[nodiscard]] constexpr auto y() noexcept -> f32& {
    return std::get<1>(components);
  }
};

extern template auto detail::Vector<Vector2f32, f32, 2>::length() const noexcept
  -> f32;
extern template auto detail::Vector<Vector2f32, f32, 2>::normalize() noexcept
  -> void;
extern template auto
  detail::Vector<Vector2f32, f32, 2>::normalize(Vector2f32) noexcept
  -> Vector2f32;
extern template auto
detail::Vector<Vector2f32, f32, 2>::distance(const Vector2f32&,
                                             const Vector2f32&) noexcept -> f32;

} // namespace basalt
