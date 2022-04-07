#include <basalt/api/math/angle.h>

#include <cmath>

namespace basalt {

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

} // namespace basalt
