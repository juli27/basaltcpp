#pragma once

#include <basalt/api/math/constants.h>

#include <basalt/api/base/types.h>

namespace basalt {

struct Angle final {
  [[nodiscard]] static constexpr auto radians(const f32 rad) -> Angle {
    return Angle {rad};
  }

  [[nodiscard]] static constexpr auto degrees(const f32 deg) -> Angle {
    return Angle {deg * RAD_PER_DEG};
  }

  constexpr Angle() noexcept = default;

  [[nodiscard]] constexpr auto radians() const noexcept -> f32 {
    return mRadians;
  }

  [[nodiscard]] constexpr auto degrees() const noexcept -> f32 {
    return mRadians * DEG_PER_RAD;
  }

  constexpr auto operator+=(const Angle rhs) -> Angle& {
    mRadians += rhs.mRadians;

    return *this;
  }

  constexpr auto operator-=(const Angle rhs) -> Angle& {
    mRadians -= rhs.mRadians;

    return *this;
  }

  [[nodiscard]] friend constexpr auto operator+(Angle lhs, const Angle rhs)
    -> Angle {
    return lhs += rhs;
  }

  [[nodiscard]] friend constexpr auto operator-(Angle lhs, const Angle rhs)
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

[[nodiscard]] constexpr auto operator"" _deg(const long double deg) noexcept
  -> Angle {
  return Angle::degrees(static_cast<f32>(deg));
}

} // namespace literals

} // namespace basalt
