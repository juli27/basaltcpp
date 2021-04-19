#pragma once

#include <basalt/api/base/vec.h>

namespace basalt {

struct Vector3f32 final : vec<Vector3f32, f32, 3> {
  constexpr Vector3f32() noexcept = default;
  constexpr Vector3f32(const f32 x, const f32 y, const f32 z) : vec {x, y, z} {
  }

  template <typename V>
  explicit constexpr Vector3f32(const vec<V, f32, 3>& o) : vec {o.x, o.y, o.z} {
  }

  [[nodiscard]] constexpr auto x() const noexcept -> f32 {
    return std::get<0>(elements);
  }

  [[nodiscard]] auto x() noexcept -> f32& {
    return std::get<0>(elements);
  }

  [[nodiscard]] constexpr auto y() const noexcept -> f32 {
    return std::get<1>(elements);
  }

  [[nodiscard]] auto y() noexcept -> f32& {
    return std::get<1>(elements);
  }

  [[nodiscard]] constexpr auto z() const noexcept -> f32 {
    return std::get<2>(elements);
  }

  [[nodiscard]] auto z() noexcept -> f32& {
    return std::get<2>(elements);
  }

  [[nodiscard]] auto length_squared() const noexcept -> f32;
  [[nodiscard]] auto length() const noexcept -> f32;

  void set(f32 ax, f32 ay, f32 az) noexcept;

  [[nodiscard]] static auto normalize(const Vector3f32&) noexcept -> Vector3f32;

  [[nodiscard]] static constexpr auto cross(const Vector3f32& l,
                                            const Vector3f32& r) noexcept
    -> Vector3f32 {
    // clang-format off
    return Vector3f32 {l.y() * r.z() - l.z() * r.y(),
                       l.z() * r.x() - l.x() * r.z(),
                       l.x() * r.y() - l.y() * r.x()};
    // clang-format on
  }

  [[nodiscard]] static auto dot(const Vector3f32&, const Vector3f32&) noexcept
    -> f32;
};

} // namespace basalt
