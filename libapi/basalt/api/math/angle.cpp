#include <basalt/api/math/angle.h>

#include <cmath>

namespace basalt {

// TODO: make Angle a literal type (constexpr) again
// either wait for C++23 (constexpr fmod) or use some compile time checked type
// as parameter

auto Angle::radians(const f32 rad) noexcept -> Angle {
  return Angle {rad};
}

auto Angle::degrees(const f32 deg) noexcept -> Angle {
  return radians(deg * RAD_PER_DEG);
}

auto Angle::arcsin(const f32 sin) noexcept -> Angle {
  return radians(std::asin(sin));
}

auto Angle::arccos(const f32 cos) noexcept -> Angle {
  return radians(std::acos(cos));
}

auto Angle::arctan(const f32 tan) noexcept -> Angle {
  return radians(std::atan(tan));
}

auto Angle::sin() const noexcept -> f32 {
  return std::sin(mRadians);
}

auto Angle::cos() const noexcept -> f32 {
  return std::cos(mRadians);
}

auto Angle::tan() const noexcept -> f32 {
  return std::tan(mRadians);
}

auto Angle::operator+=(const Angle rhs) noexcept -> Angle& {
  mRadians += rhs.mRadians;
  normalize();

  return *this;
}

auto Angle::operator-=(const Angle rhs) noexcept -> Angle& {
  mRadians -= rhs.mRadians;
  normalize();

  return *this;
}

Angle::Angle(const f32 rad) noexcept : mRadians {rad} {
  normalize();
}

auto Angle::normalize() noexcept -> void {
  constexpr f32 twoPi {2 * PI};

  mRadians = std::fmod(mRadians + PI, twoPi);
  if (mRadians < 0) {
    mRadians += twoPi;
  }

  mRadians -= PI;
}

auto literals::operator""_rad(const long double rad) noexcept -> Angle {
  return Angle::radians(static_cast<f32>(rad));
}

auto literals::operator""_rad(const unsigned long long rad) noexcept -> Angle {
  return Angle::radians(static_cast<f32>(rad));
}

auto literals::operator""_deg(const long double deg) noexcept -> Angle {
  return Angle::degrees(static_cast<f32>(deg));
}

auto literals::operator""_deg(const unsigned long long deg) noexcept -> Angle {
  return Angle::degrees(static_cast<f32>(deg));
}

auto operator+(Angle lhs, const Angle rhs) noexcept -> Angle {
  lhs += rhs;

  return lhs;
}

auto operator-(Angle lhs, const Angle rhs) noexcept -> Angle {
  lhs -= rhs;

  return lhs;
}

} // namespace basalt
