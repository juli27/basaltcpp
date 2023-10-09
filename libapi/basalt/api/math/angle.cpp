#include <basalt/api/math/angle.h>

#include <cmath>

namespace basalt {

// TODO: make Angle a literal type (constexpr) again
// either wait for C++23 (constexpr fmod) or use some compile time checked type
// as parameter

auto Angle::radians(f32 const rad) noexcept -> Angle {
  return Angle {rad};
}

auto Angle::degrees(f32 const deg) noexcept -> Angle {
  return radians(deg * sRadPerDeg);
}

auto Angle::arcsin(f32 const sin) noexcept -> Angle {
  return radians(std::asin(sin));
}

auto Angle::arccos(f32 const cos) noexcept -> Angle {
  return radians(std::acos(cos));
}

auto Angle::arctan(f32 const tan) noexcept -> Angle {
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

auto Angle::operator+=(Angle const rhs) noexcept -> Angle& {
  mRadians += rhs.mRadians;
  normalize();

  return *this;
}

auto Angle::operator-=(Angle const rhs) noexcept -> Angle& {
  mRadians -= rhs.mRadians;
  normalize();

  return *this;
}

Angle::Angle(f32 const rad) noexcept : mRadians {rad} {
  normalize();
}

auto Angle::normalize() noexcept -> void {
  constexpr auto twoPi = f32 {2.0f * PI};

  mRadians = std::fmod(mRadians + PI, twoPi);
  if (mRadians < 0) {
    mRadians += twoPi;
  }

  mRadians -= PI;
}

auto literals::operator""_rad(long double const rad) noexcept -> Angle {
  return Angle::radians(static_cast<f32>(rad));
}

auto literals::operator""_rad(unsigned long long const rad) noexcept -> Angle {
  return Angle::radians(static_cast<f32>(rad));
}

auto literals::operator""_deg(long double const deg) noexcept -> Angle {
  return Angle::degrees(static_cast<f32>(deg));
}

auto literals::operator""_deg(unsigned long long const deg) noexcept -> Angle {
  return Angle::degrees(static_cast<f32>(deg));
}

auto operator+(Angle lhs, Angle const rhs) noexcept -> Angle {
  lhs += rhs;

  return lhs;
}

auto operator-(Angle lhs, Angle const rhs) noexcept -> Angle {
  lhs -= rhs;

  return lhs;
}

} // namespace basalt
