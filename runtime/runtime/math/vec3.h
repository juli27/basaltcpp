#pragma once

#include "runtime/shared/types.h"

namespace basalt {

template <typename T>
struct Vec3 final {
  T x {};
  T y {};
  T z {};

  constexpr Vec3() noexcept = default;

  constexpr Vec3(const T xVal, const T yVal, const T zVal) noexcept
    : x {xVal}, y {yVal}, z {zVal} {
  }

  auto operator+=(const Vec3&) noexcept -> Vec3&;
  auto operator-=(const Vec3&) noexcept -> Vec3&;
  auto operator*=(T) noexcept -> Vec3&;
  auto operator/=(T) noexcept -> Vec3&;

  constexpr auto operator-() const noexcept -> Vec3 {
    return Vec3 {-x, -y, -z};
  }

  [[nodiscard]] auto operator==(const Vec3&) const noexcept -> bool;

  [[nodiscard]] auto operator!=(const Vec3&) const noexcept -> bool;

  [[nodiscard]] auto length_squared() const noexcept -> T;

  [[nodiscard]] auto length() const noexcept -> T;

  void set(T ax, T ay, T az) noexcept;

  [[nodiscard]] static auto normalize(const Vec3&) noexcept -> Vec3;

  [[nodiscard]] static constexpr auto cross(const Vec3& l,
                                            const Vec3& r) noexcept -> Vec3 {
    // clang-format off
    return Vec3 {l.y * r.z - l.z * r.y,
                 l.z * r.x - l.x * r.z,
                 l.x * r.y - l.y * r.x};
    // clang-format on
  }

  [[nodiscard]] static auto dot(const Vec3&, const Vec3&) noexcept -> T;
};

extern template struct Vec3<f32>;

using Vec3f32 = Vec3<f32>;

template <typename T>
auto operator+(Vec3<T>, const Vec3<T>&) noexcept -> Vec3<T>;

template <typename T>
auto operator-(Vec3<T>, const Vec3<T>&) noexcept -> Vec3<T>;

template <typename T>
auto operator*(Vec3<T>, T) noexcept -> Vec3<T>;

template <typename T>
auto operator*(T, const Vec3<T>&) noexcept -> Vec3<T>;

template <typename T>
auto operator/(Vec3<T>, T) noexcept -> Vec3<T>;

} // namespace basalt
