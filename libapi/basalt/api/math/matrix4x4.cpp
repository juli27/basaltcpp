#include <basalt/api/math/matrix4x4.h>

#include <basalt/api/math/angle.h>

#include <cmath>

namespace basalt {

auto Matrix4x4f32::rotation_x(const Angle angle) noexcept -> Matrix4x4f32 {
  Matrix4x4f32 result {identity()};

  result.m22 = result.m33 = angle.cos();
  result.m23 = angle.sin();
  result.m32 = -result.m23;

  return result;
}

auto Matrix4x4f32::rotation_y(const Angle angle) noexcept -> Matrix4x4f32 {
  Matrix4x4f32 result {identity()};

  result.m11 = result.m33 = angle.cos();
  result.m31 = angle.sin();
  result.m13 = -result.m31;

  return result;
}

auto Matrix4x4f32::rotation_z(const Angle angle) noexcept -> Matrix4x4f32 {
  Matrix4x4f32 result {identity()};

  result.m11 = result.m22 = angle.cos();
  result.m12 = angle.sin();
  result.m21 = -result.m12;

  return result;
}

auto Matrix4x4f32::rotation(const Vector3f32& radians) noexcept
  -> Matrix4x4f32 {
  const Matrix4x4f32 x {rotation_x(Angle::radians(radians.x()))};
  const Matrix4x4f32 y {rotation_y(Angle::radians(radians.y()))};
  const Matrix4x4f32 z {rotation_z(Angle::radians(radians.z()))};

  return z * x * y;
}

auto Matrix4x4f32::perspective_projection(const Angle fov,
                                          const f32 aspectRatio,
                                          const f32 nearPlane,
                                          const f32 farPlane) noexcept
  -> Matrix4x4f32 {
  const f32 yScale {1.0f / std::tan(fov.radians() * 0.5f)};
  const f32 xScale {yScale / aspectRatio};
  const f32 q {farPlane / (farPlane - nearPlane)};

  // clang-format off
  return Matrix4x4f32 {
    xScale,   0.0f,           0.0f, 0.0f,
      0.0f, yScale,           0.0f, 0.0f,
      0.0f,   0.0f,              q, 1.0f,
      0.0f,   0.0f, -q * nearPlane, 0.0f
  };
  // clang-format on
}

} // namespace basalt
