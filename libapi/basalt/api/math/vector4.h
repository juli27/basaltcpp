#pragma once

#include <basalt/api/math/vector.h>

#include <basalt/api/math/types.h>

#include <basalt/api/base/types.h>

#include <array>

namespace basalt {

class Vector4f32 : public detail::Vector<Vector4f32, f32, 4> {
public:
  using Vector::normalize;
  [[nodiscard]] static auto normalize(f32 x, f32 y, f32 z, f32 w) noexcept
    -> Vector4f32;

  // sets every component to 0
  constexpr Vector4f32() noexcept = default;

  // sets every component to v
  constexpr explicit Vector4f32(const f32 v) noexcept : Vector {v, v, v, v} {
  }

  constexpr Vector4f32(const f32 x, const f32 y, const f32 z,
                       const f32 w) noexcept
    : Vector {x, y, z, w} {
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

  [[nodiscard]] constexpr auto z() const noexcept -> f32 {
    return std::get<2>(components);
  }

  [[nodiscard]] constexpr auto z() noexcept -> f32& {
    return std::get<2>(components);
  }

  [[nodiscard]] constexpr auto w() const noexcept -> f32 {
    return std::get<3>(components);
  }

  [[nodiscard]] constexpr auto w() noexcept -> f32& {
    return std::get<3>(components);
  }
};

extern template auto detail::Vector<Vector4f32, f32, 4>::length() const noexcept
  -> f32;
extern template auto
detail::Vector<Vector4f32, f32, 4>::normalize() const noexcept -> Vector4f32;
extern template auto
  detail::Vector<Vector4f32, f32, 4>::normalize(Vector4f32) noexcept
  -> Vector4f32;
extern template auto
detail::Vector<Vector4f32, f32, 4>::distance(const Vector4f32&,
                                             const Vector4f32&) noexcept -> f32;

} // namespace basalt
