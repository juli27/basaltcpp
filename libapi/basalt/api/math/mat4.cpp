#include <basalt/api/math/mat4.h>

#include <cmath>

namespace basalt {

auto Mat4::rotation_x(const f32 radians) noexcept -> Mat4 {
  Mat4 result {identity()};

  result.m22 = result.m33 = std::cos(radians);
  result.m23 = std::sin(radians);
  result.m32 = -result.m23;

  return result;
}

auto Mat4::rotation_y(const f32 radians) noexcept -> Mat4 {
  Mat4 result {identity()};

  result.m11 = result.m33 = std::cos(radians);
  result.m31 = std::sin(radians);
  result.m13 = -result.m31;

  return result;
}

auto Mat4::rotation_z(const f32 radians) noexcept -> Mat4 {
  Mat4 result {identity()};

  result.m11 = result.m22 = std::cos(radians);
  result.m12 = std::sin(radians);
  result.m21 = -result.m12;

  return result;
}

auto Mat4::rotation(const Vector3f32& radians) noexcept -> Mat4 {
  const Mat4 x {rotation_x(radians.x())};
  const Mat4 y {rotation_y(radians.y())};
  const Mat4 z {rotation_z(radians.z())};

  return z * x * y;
}

auto Mat4::perspective_projection(const f32 fovRadians, const f32 aspectRatio,
                                  const f32 nearPlane,
                                  const f32 farPlane) noexcept -> Mat4 {
  const auto yScale = 1.0f / std::tan(fovRadians * 0.5f);
  const auto xScale = yScale / aspectRatio;
  const auto q = farPlane / (farPlane - nearPlane);

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
