#pragma once

#include <basalt/api/math/vector.h>

#include <basalt/api/math/types.h>

#include <basalt/api/base/types.h>

#include <array>

namespace basalt {

class Vector3f32 : public detail::Vector<Vector3f32, f32, 3> {
public:
  [[nodiscard]] static constexpr auto right() noexcept -> Vector3f32 {
    return Vector3f32 {1.0f, 0.0f, 0.0f};
  }

  [[nodiscard]] static constexpr auto left() noexcept -> Vector3f32 {
    return Vector3f32 {-1.0f, 0.0f, 0.0f};
  }

  [[nodiscard]] static constexpr auto up() noexcept -> Vector3f32 {
    return Vector3f32 {0.0f, 1.0f, 0.0f};
  }

  [[nodiscard]] static constexpr auto down() noexcept -> Vector3f32 {
    return Vector3f32 {0.0f, -1.0f, 0.0f};
  }

  [[nodiscard]] static constexpr auto forward() noexcept -> Vector3f32 {
    return Vector3f32 {0.0f, 0.0f, 1.0f};
  }

  [[nodiscard]] static constexpr auto back() noexcept -> Vector3f32 {
    return Vector3f32 {0.0f, 0.0f, -1.0f};
  }

  using Vector::normalize;
  [[nodiscard]] static auto normalize(f32 x, f32 y, f32 z) noexcept
    -> Vector3f32;

  // sets every component to 0
  constexpr Vector3f32() noexcept = default;

  // sets every component to v
  constexpr explicit Vector3f32(const f32 v) noexcept : Vector {v, v, v} {
  }

  constexpr Vector3f32(const f32 x, const f32 y, const f32 z) noexcept
    : Vector {x, y, z} {
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

  [[nodiscard]] static constexpr auto cross(const Vector3f32& l,
                                            const Vector3f32& r) noexcept
    -> Vector3f32 {
    return Vector3f32 {
      l.y() * r.z() - l.z() * r.y(),
      l.z() * r.x() - l.x() * r.z(),
      l.x() * r.y() - l.y() * r.x(),
    };
  }

  [[nodiscard]] static auto angle(const Vector3f32&, const Vector3f32&)
    -> Angle;
};

extern template auto detail::Vector<Vector3f32, f32, 3>::length() const noexcept
  -> f32;
extern template auto detail::Vector<Vector3f32, f32, 3>::normalize() noexcept
  -> void;
extern template auto
  detail::Vector<Vector3f32, f32, 3>::normalize(Vector3f32) noexcept
  -> Vector3f32;
extern template auto
detail::Vector<Vector3f32, f32, 3>::distance(const Vector3f32&,
                                             const Vector3f32&) noexcept -> f32;

} // namespace basalt
