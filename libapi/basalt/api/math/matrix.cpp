#include "matrix2.h"
#include "matrix3.h"
#include "matrix4.h"

#include "angle.h"
#include "vector3.h"

#include <cmath>

namespace basalt {

auto Matrix2x2f32::rotation(Angle const angle) -> Matrix2x2f32 {
  auto const cosAngle = angle.cos();
  auto const sinAngle = angle.sin();

  // clang-format off
  return Matrix2x2f32{ cosAngle, sinAngle,
                      -sinAngle, cosAngle};
  // clang-format on
}

auto Matrix3x3f32::rotation(Angle const x, Angle const y, Angle const z)
  -> Matrix3x3f32 {
  return rotation_y(y) * rotation_x(x) * rotation_z(z);
}

auto Matrix3x3f32::rotation_x(Angle const angle) -> Matrix3x3f32 {
  auto const cosAngle = angle.cos();
  auto const sinAngle = angle.sin();

  // clang-format off
  return Matrix3x3f32{1.0f,      0.0f,     0.0f,
                      0.0f,  cosAngle, sinAngle,
                      0.0f, -sinAngle, cosAngle};
  // clang-format on
}

auto Matrix3x3f32::rotation_y(Angle const angle) -> Matrix3x3f32 {
  auto const cosAngle = angle.cos();
  auto const sinAngle = angle.sin();

  // clang-format off
  return Matrix3x3f32{cosAngle, 0.0f, -sinAngle,
                          0.0f, 1.0f,      0.0f,
                      sinAngle, 0.0f,  cosAngle};
  // clang-format on
}

auto Matrix3x3f32::rotation_z(Angle const angle) -> Matrix3x3f32 {
  auto const cosAngle = angle.cos();
  auto const sinAngle = angle.sin();

  // clang-format off
  return Matrix3x3f32{ cosAngle, sinAngle, 0.0f,
                      -sinAngle, cosAngle, 0.0f,
                           0.0f,     0.0f, 1.0f};
  // clang-format on
}

auto Matrix3x3f32::rotation_axis(Vector3f32 const& axis, Angle const angle)
  -> Matrix3x3f32 {
  auto const cos = angle.cos();
  auto const sin = angle.sin();
  auto const oneMinusCos = 1.0f - cos;

  // clang-format off
  return Matrix3x3f32{
    axis.x() * axis.x() * oneMinusCos + cos,
    axis.x() * axis.y() * oneMinusCos + axis.z() * sin,
    axis.x() * axis.z() * oneMinusCos - axis.y() * sin,

    axis.y() * axis.x() * oneMinusCos - axis.z() * sin,
    axis.y() * axis.y() * oneMinusCos + cos,
    axis.y() * axis.z() * oneMinusCos + axis.x() * sin,

    axis.z() * axis.x() * oneMinusCos + axis.y() * sin,
    axis.z() * axis.y() * oneMinusCos - axis.x() * sin,
    axis.z() * axis.z() * oneMinusCos + cos,
  };
  // clang-format on
}

auto Matrix4x4f32::look_at_lh(Vector3f32 const& position,
                              Vector3f32 const& lookAt, Vector3f32 const& up)
  -> Matrix4x4f32 {
  auto const zAxis = Vector3f32::normalized(lookAt - position);
  auto const xAxis = Vector3f32::normalized(Vector3f32::cross(up, zAxis));
  auto const yAxis = Vector3f32::normalized(Vector3f32::cross(zAxis, xAxis));

  return translation(-position) *
         Matrix4x4f32{xAxis.x(), yAxis.x(), zAxis.x(), 0.0f,
                      xAxis.y(), yAxis.y(), zAxis.y(), 0.0f,
                      xAxis.z(), yAxis.z(), zAxis.z(), 0.0f,
                      0.0f,      0.0f,      0.0f,      1.0f};
}

auto Matrix4x4f32::perspective_projection(Angle const fov,
                                          f32 const aspectRatio,
                                          f32 const nearPlaneZ,
                                          f32 const farPlaneZ) -> Matrix4x4f32 {
  auto const yScale = 1.0f / std::tan(fov.radians() * 0.5f);
  auto const q = farPlaneZ / (farPlaneZ - nearPlaneZ);

  // clang-format off
  return Matrix4x4f32{
    yScale / aspectRatio, 0.0f,   0.0f,            0.0f,
    0.0f,                 yScale, 0.0f,            0.0f,
    0.0f,                 0.0f,   q,               1.0f,
    0.0f,                 0.0f,   -q * nearPlaneZ, 0.0f,
  };
  // clang-format on
}

} // namespace basalt
