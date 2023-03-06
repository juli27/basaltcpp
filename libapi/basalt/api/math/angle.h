#pragma once

#include <basalt/api/math/constants.h>

#include <basalt/api/base/types.h>

namespace basalt {

struct Angle final {
  [[nodiscard]] static constexpr auto radians(const f32 rad) noexcept -> Angle {
    return Angle {rad};
  }

  [[nodiscard]] static constexpr auto degrees(const f32 deg) noexcept -> Angle {
    return radians(deg * RAD_PER_DEG);
  }

  // sin in [-1; 1]
  [[nodiscard]] static auto arcsin(f32 sin) noexcept -> Angle;

  // cos in [-1; 1]
  [[nodiscard]] static auto arccos(f32 cos) noexcept -> Angle;

  [[nodiscard]] static auto arctan(f32 tan) noexcept -> Angle;

  constexpr Angle() noexcept = default;

  [[nodiscard]] constexpr auto radians() const noexcept -> f32 {
    return mRadians;
  }

  [[nodiscard]] constexpr auto degrees() const noexcept -> f32 {
    return mRadians * DEG_PER_RAD;
  }

  [[nodiscard]] auto sin() const noexcept -> f32;
  [[nodiscard]] auto cos() const noexcept -> f32;
  [[nodiscard]] auto tan() const noexcept -> f32;

  constexpr auto operator+=(const Angle rhs) noexcept -> Angle& {
    mRadians += rhs.mRadians;

    return *this;
  }

  constexpr auto operator-=(const Angle rhs) noexcept -> Angle& {
    mRadians -= rhs.mRadians;

    return *this;
  }

  [[nodiscard]] friend constexpr auto operator+(Angle lhs,
                                                const Angle rhs) noexcept
    -> Angle {
    return lhs += rhs;
  }

  [[nodiscard]] friend constexpr auto operator-(Angle lhs,
                                                const Angle rhs) noexcept
    -> Angle {
    return lhs -= rhs;
  }

private:
  f32 mRadians {};

  explicit constexpr Angle(const f32 rad) noexcept : mRadians {rad} {
  }

  static constexpr f32 DEG_PER_RAD {180.0f / PI};
  static constexpr f32 RAD_PER_DEG {PI / 180.0f};
};

inline namespace literals {

[[nodiscard]] constexpr auto operator"" _rad(const long double rad) noexcept
  -> Angle {
  return Angle::radians(static_cast<f32>(rad));
}

[[nodiscard]] constexpr auto
operator"" _rad(const unsigned long long rad) noexcept -> Angle {
  return Angle::radians(static_cast<f32>(rad));
}

[[nodiscard]] constexpr auto operator"" _deg(const long double deg) noexcept
  -> Angle {
  return Angle::degrees(static_cast<f32>(deg));
}

[[nodiscard]] constexpr auto
operator"" _deg(const unsigned long long deg) noexcept -> Angle {
  return Angle::degrees(static_cast<f32>(deg));
}

} // namespace literals

} // namespace basalt
