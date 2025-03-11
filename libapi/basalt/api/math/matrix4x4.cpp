#include "matrix4x4.h"

#include "angle.h"

#include <cmath>

namespace basalt {

auto Matrix4x4f32::rotation_x(Angle const angle) noexcept -> Matrix4x4f32 {
  auto const cos = angle.cos();
  auto const sin = angle.sin();

  // clang-format off
  return Matrix4x4f32{
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, cos,  sin,  0.0f,
    0.0f, -sin, cos,  0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
  };
  // clang-format on
}

auto Matrix4x4f32::rotation_y(Angle const angle) noexcept -> Matrix4x4f32 {
  auto const cos = angle.cos();
  auto const sin = angle.sin();

  // clang-format off
  return Matrix4x4f32{
    cos,  0.0f, -sin, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    sin,  0.0f, cos,  0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
  };
  // clang-format on
}

auto Matrix4x4f32::rotation_z(Angle const angle) noexcept -> Matrix4x4f32 {
  auto const cos = angle.cos();
  auto const sin = angle.sin();

  // clang-format off
  return Matrix4x4f32{
    cos,  sin,  0.0f, 0.0f,
    -sin, cos,  0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
  };
  // clang-format on
}

auto Matrix4x4f32::rotation(Angle const x, Angle const y,
                            Angle const z) noexcept -> Matrix4x4f32 {
  return rotation_z(z) * rotation_x(x) * rotation_y(y);
}

auto Matrix4x4f32::rotation(Vector3f32 const& radians) noexcept
  -> Matrix4x4f32 {
  return rotation(Angle::radians(radians.x()), Angle::radians(radians.y()),
                  Angle::radians(radians.z()));
}

auto Matrix4x4f32::rotation(Vector3f32 const& axis, Angle const angle) noexcept
  -> Matrix4x4f32 {
  auto const cos = angle.cos();
  auto const sin = -angle.sin();
  auto const oneMinusCos = 1.0f - cos;

  auto const a = axis.normalize();

  // clang-format off
  return Matrix4x4f32{
    a.x() * a.x() * oneMinusCos + cos,
    a.x() * a.y() * oneMinusCos - a.z() * sin,
    a.x() * a.z() * oneMinusCos + a.y() * sin,
    0.0f,
    a.y() * a.x() * oneMinusCos + a.z() * sin,
    a.y() * a.y() * oneMinusCos + cos,
    a.y() * a.z() * oneMinusCos - a.x() * sin,
    0.0f,
    a.z() * a.x() * oneMinusCos - a.y() * sin,
    a.z() * a.y() * oneMinusCos + a.x() * sin,
    a.z() * a.z() * oneMinusCos + cos,
    0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
  };
  // clang-format on
}

auto Matrix4x4f32::look_at_lh(Vector3f32 const& position,
                              Vector3f32 const& lookAt, Vector3f32 const& up)
  -> Matrix4x4f32 {
  auto const zAxis = (lookAt - position).normalize();
  auto const xAxis = up.cross(zAxis).normalize();
  auto const yAxis = zAxis.cross(xAxis).normalize();

  return translation(-position) *
         Matrix4x4f32{xAxis.x(), yAxis.x(), zAxis.x(), 0.0f,
                      xAxis.y(), yAxis.y(), zAxis.y(), 0.0f,
                      xAxis.z(), yAxis.z(), zAxis.z(), 0.0f,
                      0.0f,      0.0f,      0.0f,      1.0f};
}

auto Matrix4x4f32::perspective_projection(Angle const fov,
                                          f32 const aspectRatio,
                                          f32 const nearPlaneZ,
                                          f32 const farPlaneZ) noexcept
  -> Matrix4x4f32 {
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

// TODO: investigate inverted() (last column?)

} // namespace basalt
