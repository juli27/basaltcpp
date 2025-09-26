#pragma once

#include "constants.h"

#include <basalt/api/base/types.h>

namespace basalt {

class Angle {
public:
  [[nodiscard]]
  static auto radians(f32 rad) noexcept -> Angle;

  [[nodiscard]]
  static auto degrees(f32 deg) noexcept -> Angle;

  // sin in [-1; 1]
  [[nodiscard]]
  static auto arcsin(f32 sin) noexcept -> Angle;

  // cos in [-1; 1]
  [[nodiscard]]
  static auto arccos(f32 cos) noexcept -> Angle;

  [[nodiscard]]
  static auto arctan(f32 tan) noexcept -> Angle;

  constexpr Angle() noexcept = default;

  [[nodiscard]]
  constexpr auto radians() const noexcept -> f32 {
    return mRadians;
  }

  [[nodiscard]]
  constexpr auto degrees() const noexcept -> f32 {
    return radToDeg(mRadians);
  }

  [[nodiscard]]
  auto sin() const noexcept -> f32;

  [[nodiscard]]
  auto cos() const noexcept -> f32;

  [[nodiscard]]
  auto tan() const noexcept -> f32;

  auto operator+=(Angle rhs) noexcept -> Angle&;
  [[nodiscard]] friend auto operator+(Angle lhs, Angle rhs) noexcept -> Angle;

  auto operator-=(Angle rhs) noexcept -> Angle&;
  [[nodiscard]] friend auto operator-(Angle lhs, Angle rhs) noexcept -> Angle;

  static constexpr auto radToDeg(f32 const rad) -> f32 {
    return rad * sDegPerRad;
  }

  static constexpr auto degToRad(f32 const deg) -> f32 {
    return deg * sRadPerDeg;
  }

private:
  f32 mRadians{};

  explicit Angle(f32 rad) noexcept;

  // normalize angle to [-pi,pi)
  auto normalize() noexcept -> void;

  static constexpr auto sDegPerRad = 180.0f / PI;
  static constexpr auto sRadPerDeg = PI / 180.0f;
};

inline namespace literals {

[[nodiscard]] auto operator"" _rad(long double rad) noexcept -> Angle;
[[nodiscard]] auto operator"" _rad(unsigned long long rad) noexcept -> Angle;
[[nodiscard]] auto operator"" _deg(long double deg) noexcept -> Angle;
[[nodiscard]] auto operator"" _deg(unsigned long long deg) noexcept -> Angle;

} // namespace literals

} // namespace basalt
