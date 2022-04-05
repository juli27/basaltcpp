#include <basalt/api/math/mat4.h>

#include <basalt/api/math/angle.h>

#include <cmath>

namespace basalt {

auto Mat4::rotation_x(const Angle angle) noexcept -> Mat4 {
  Mat4 result {identity()};

  result.m22 = result.m33 = std::cos(angle.radians());
  result.m23 = std::sin(angle.radians());
  result.m32 = -result.m23;

  return result;
}

auto Mat4::rotation_y(const Angle angle) noexcept -> Mat4 {
  Mat4 result {identity()};

  result.m11 = result.m33 = std::cos(angle.radians());
  result.m31 = std::sin(angle.radians());
  result.m13 = -result.m31;

  return result;
}

auto Mat4::rotation_z(const Angle angle) noexcept -> Mat4 {
  Mat4 result {identity()};

  result.m11 = result.m22 = std::cos(angle.radians());
  result.m12 = std::sin(angle.radians());
  result.m21 = -result.m12;

  return result;
}

auto Mat4::rotation(const Vector3f32& radians) noexcept -> Mat4 {
  const Mat4 x {rotation_x(Angle::radians(radians.x()))};
  const Mat4 y {rotation_y(Angle::radians(radians.y()))};
  const Mat4 z {rotation_z(Angle::radians(radians.z()))};

  return z * x * y;
}

auto Mat4::perspective_projection(const Angle fov, const f32 aspectRatio,
                                  const f32 nearPlane,
                                  const f32 farPlane) noexcept -> Mat4 {
  const f32 yScale {1.0f / std::tan(fov.radians() * 0.5f)};
  const f32 xScale {yScale / aspectRatio};
  const f32 q {farPlane / (farPlane - nearPlane)};

  // clang-format off
  return Mat4 {
    xScale,   0.0f,           0.0f, 0.0f,
      0.0f, yScale,           0.0f, 0.0f,
      0.0f,   0.0f,              q, 1.0f,
      0.0f,   0.0f, -q * nearPlane, 0.0f
  };
  // clang-format on
}

} // namespace basalt
