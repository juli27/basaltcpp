#include <basalt/api/math/matrix4x4.h>

#include <basalt/api/math/angle.h>

#include <cmath>

namespace basalt {

auto Matrix4x4f32::rotation_x(const Angle angle) noexcept -> Matrix4x4f32 {
  const f32 cos {angle.cos()};
  const f32 sin {angle.sin()};

  // clang-format off
  return Matrix4x4f32 {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, cos,  sin,  0.0f,
    0.0f, -sin, cos,  0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
  };
  // clang-format on
}

auto Matrix4x4f32::rotation_y(const Angle angle) noexcept -> Matrix4x4f32 {
  const f32 cos {angle.cos()};
  const f32 sin {angle.sin()};

  // clang-format off
  return Matrix4x4f32 {
    cos,  0.0f, -sin, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    sin,  0.0f, cos,  0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
  };
  // clang-format on
}

auto Matrix4x4f32::rotation_z(const Angle angle) noexcept -> Matrix4x4f32 {
  const f32 cos {angle.cos()};
  const f32 sin {angle.sin()};

  // clang-format off
  return Matrix4x4f32 {
    cos,  sin,  0.0f, 0.0f,
    -sin, cos,  0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
  };
  // clang-format on
}

auto Matrix4x4f32::rotation(const Angle x, const Angle y,
                            const Angle z) noexcept -> Matrix4x4f32 {
  return rotation_z(z) * rotation_x(x) * rotation_y(y);
}

auto Matrix4x4f32::rotation(const Vector3f32& radians) noexcept
  -> Matrix4x4f32 {
  return rotation(Angle::radians(radians.x()), Angle::radians(radians.y()),
                  Angle::radians(radians.z()));
}

auto Matrix4x4f32::rotation(const Vector3f32& axis, const Angle angle) noexcept
  -> Matrix4x4f32 {
  const f32 cos {angle.cos()};
  const f32 sin {-angle.sin()};
  const f32 oneMinusCos {1.0f - cos};

  const auto a {Vector3f32::normalize(axis)};

  // clang-format off
  return Matrix4x4f32 {
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

auto Matrix4x4f32::look_at_lh(const Vector3f32& position,
                              const Vector3f32& lookAt, const Vector3f32& up)
  -> Matrix4x4f32 {
  const auto zAxis {Vector3f32::normalize(lookAt - position)};
  const auto xAxis {Vector3f32::normalize(Vector3f32::cross(up, zAxis))};
  const auto yAxis {Vector3f32::normalize(Vector3f32::cross(zAxis, xAxis))};

  return translation(-position) *
         Matrix4x4f32 {xAxis.x(), yAxis.x(), zAxis.x(), 0.0f,
                       xAxis.y(), yAxis.y(), zAxis.y(), 0.0f,
                       xAxis.z(), yAxis.z(), zAxis.z(), 0.0f,
                       0.0f,      0.0f,      0.0f,      1.0f};
}

auto Matrix4x4f32::perspective_projection(const Angle fov,
                                          const f32 aspectRatio,
                                          const f32 nearPlaneZ,
                                          const f32 farPlaneZ) noexcept
  -> Matrix4x4f32 {
  const f32 yScale {1.0f / std::tan(fov.radians() * 0.5f)};
  const f32 q {farPlaneZ / (farPlaneZ - nearPlaneZ)};

  // clang-format off
  return Matrix4x4f32 {
    yScale / aspectRatio, 0.0f,   0.0f,            0.0f,
    0.0f,                 yScale, 0.0f,            0.0f,
    0.0f,                 0.0f,   q,               1.0f,
    0.0f,                 0.0f,   -q * nearPlaneZ, 0.0f,
  };
  // clang-format on
}

// TODO: investigate inverted() (last column?)

} // namespace basalt
