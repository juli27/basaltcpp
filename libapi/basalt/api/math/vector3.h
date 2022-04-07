#pragma once

#include <basalt/api/base/vec.h>

#include <basalt/api/math/types.h>

#include <basalt/api/base/types.h>

#include <array>

namespace basalt {

struct Vector3f32 final : vec<Vector3f32, f32, 3> {
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

  constexpr explicit Vector3f32(const f32 value) noexcept
    : vec {value, value, value} {
  }

  constexpr Vector3f32(const f32 x, const f32 y, const f32 z) noexcept
    : vec {x, y, z} {
  }

  [[nodiscard]] constexpr auto x() const noexcept -> f32 {
    return std::get<0>(elements);
  }

  [[nodiscard]] constexpr auto x() noexcept -> f32& {
    return std::get<0>(elements);
  }

  [[nodiscard]] constexpr auto y() const noexcept -> f32 {
    return std::get<1>(elements);
  }

  [[nodiscard]] constexpr auto y() noexcept -> f32& {
    return std::get<1>(elements);
  }

  [[nodiscard]] constexpr auto z() const noexcept -> f32 {
    return std::get<2>(elements);
  }

  [[nodiscard]] constexpr auto z() noexcept -> f32& {
    return std::get<2>(elements);
  }

  [[nodiscard]] constexpr auto length_squared() const noexcept -> f32 {
    return x() * x() + y() * y() + z() * z();
  }

  [[nodiscard]] auto length() const noexcept -> f32;

  constexpr auto set(const f32 value) noexcept -> void {
    set(value, value, value);
  }

  constexpr auto set(const f32 x, const f32 y, const f32 z) noexcept -> void {
    this->x() = x;
    this->y() = y;
    this->z() = z;
  }

  // length = 0 -> undefined
  [[nodiscard]] static auto normalize(const Vector3f32&) -> Vector3f32;

  [[nodiscard]] static constexpr auto cross(const Vector3f32& l,
                                            const Vector3f32& r) noexcept
    -> Vector3f32 {
    return Vector3f32 {
      l.y() * r.z() - l.z() * r.y(),
      l.z() * r.x() - l.x() * r.z(),
      l.x() * r.y() - l.y() * r.x(),
    };
  }

  [[nodiscard]] static constexpr auto dot(const Vector3f32& l,
                                          const Vector3f32& r) noexcept -> f32 {
    return l.x() * r.x() + l.y() * r.y() + l.z() * r.z();
  }

  [[nodiscard]] static auto angle(const Vector3f32&, const Vector3f32&)
    -> Angle;
};

} // namespace basalt
