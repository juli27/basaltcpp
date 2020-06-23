#include "mat4.h"

#include <cmath>

namespace basalt {

auto Mat4::operator+=(const Mat4& rhs) noexcept -> Mat4& {
  m11 += rhs.m11; m12 += rhs.m12; m13 += rhs.m13; m14 += rhs.m14;
  m21 += rhs.m21; m22 += rhs.m22; m23 += rhs.m23; m24 += rhs.m24;
  m31 += rhs.m31; m32 += rhs.m32; m33 += rhs.m33; m34 += rhs.m34;
  m41 += rhs.m41; m42 += rhs.m42; m43 += rhs.m43; m44 += rhs.m44;

  return *this;
}

auto Mat4::operator-=(const Mat4& rhs) noexcept -> Mat4& {
  m11 -= rhs.m11; m12 -= rhs.m12; m13 -= rhs.m13; m14 -= rhs.m14;
  m21 -= rhs.m21; m22 -= rhs.m22; m23 -= rhs.m23; m24 -= rhs.m24;
  m31 -= rhs.m31; m32 -= rhs.m32; m33 -= rhs.m33; m34 -= rhs.m34;
  m41 -= rhs.m41; m42 -= rhs.m42; m43 -= rhs.m43; m44 -= rhs.m44;

  return *this;
}

auto Mat4::operator*=(const Mat4& rhs) noexcept -> Mat4& {
  *this = Mat4(
    rhs.m11 * m11 + rhs.m21 * m12 + rhs.m31 * m13 + rhs.m41 * m14,
    rhs.m12 * m11 + rhs.m22 * m12 + rhs.m32 * m13 + rhs.m42 * m14,
    rhs.m13 * m11 + rhs.m23 * m12 + rhs.m33 * m13 + rhs.m43 * m14,
    rhs.m14 * m11 + rhs.m24 * m12 + rhs.m34 * m13 + rhs.m44 * m14,

    rhs.m11 * m21 + rhs.m21 * m22 + rhs.m31 * m23 + rhs.m41 * m24,
    rhs.m12 * m21 + rhs.m22 * m22 + rhs.m32 * m23 + rhs.m42 * m24,
    rhs.m13 * m21 + rhs.m23 * m22 + rhs.m33 * m23 + rhs.m43 * m24,
    rhs.m14 * m21 + rhs.m24 * m22 + rhs.m34 * m23 + rhs.m44 * m24,

    rhs.m11 * m31 + rhs.m21 * m32 + rhs.m31 * m33 + rhs.m41 * m34,
    rhs.m12 * m31 + rhs.m22 * m32 + rhs.m32 * m33 + rhs.m42 * m34,
    rhs.m13 * m31 + rhs.m23 * m32 + rhs.m33 * m33 + rhs.m43 * m34,
    rhs.m14 * m31 + rhs.m24 * m32 + rhs.m34 * m33 + rhs.m44 * m34,

    rhs.m11 * m41 + rhs.m21 * m42 + rhs.m31 * m43 + rhs.m41 * m44,
    rhs.m12 * m41 + rhs.m22 * m42 + rhs.m32 * m43 + rhs.m42 * m44,
    rhs.m13 * m41 + rhs.m23 * m42 + rhs.m33 * m43 + rhs.m43 * m44,
    rhs.m14 * m41 + rhs.m24 * m42 + rhs.m34 * m43 + rhs.m44 * m44
  );

  return *this;
}

auto Mat4::operator*=(const f32 rhs) noexcept -> Mat4& {
  m11 *= rhs; m12 *= rhs; m13 *= rhs; m14 *= rhs;
  m21 *= rhs; m22 *= rhs; m23 *= rhs; m24 *= rhs;
  m31 *= rhs; m32 *= rhs; m33 *= rhs; m34 *= rhs;
  m41 *= rhs; m42 *= rhs; m43 *= rhs; m44 *= rhs;

  return *this;
}

auto Mat4::operator/=(const Mat4& rhs) noexcept -> Mat4& {
  *this *= invert(rhs);
  return *this;
}

auto Mat4::operator/=(const f32 rhs) noexcept -> Mat4& {
  m11 /= rhs; m12 /= rhs; m13 /= rhs; m14 /= rhs;
  m21 /= rhs; m22 /= rhs; m23 /= rhs; m24 /= rhs;
  m31 /= rhs; m32 /= rhs; m33 /= rhs; m34 /= rhs;
  m41 /= rhs; m42 /= rhs; m43 /= rhs; m44 /= rhs;

  return *this;
}

auto Mat4::invert(const Mat4& m) noexcept -> Mat4 {
  auto invDet = m.det();
  if (invDet == 0.0f) {
    return identity();
  }

  invDet = 1.0f / invDet;

  Mat4 result {};
  result.m11 = invDet * (m.m22 * m.m33 - m.m23 * m.m32);
  result.m12 = -invDet * (m.m12 * m.m33 - m.m13 * m.m32);
  result.m13 = invDet * (m.m12 * m.m23 - m.m13 * m.m22);

  result.m21 = -invDet * (m.m21 * m.m33 - m.m23 * m.m31);
  result.m22 = invDet * (m.m11 * m.m33 - m.m13 * m.m31);
  result.m23 = -invDet * (m.m11 * m.m23 - m.m13 * m.m21);

  result.m31 = invDet * (m.m21 * m.m32 - m.m22 * m.m31);
  result.m32 = -invDet * (m.m11 * m.m32 - m.m12 * m.m31);
  result.m33 = invDet * (m.m11 * m.m22 - m.m12 * m.m21);

  result.m41 = -(m.m41 * result.m11 + m.m42 * result.m21 + m.m43 * result.m31);
  result.m42 = -(m.m41 * result.m12 + m.m42 * result.m22 + m.m43 * result.m32);
  result.m43 = -(m.m41 * result.m13 + m.m42 * result.m23 + m.m43 * result.m33);
  result.m44 = 1.0f;

  return result;
}

auto Mat4::rotation_x(const f32 radians) -> Mat4 {
  auto result = identity();

  result.m22 = result.m33 = std::cos(radians);
  result.m23 = std::sin(radians);
  result.m32 = -result.m23;

  return result;
}

auto Mat4::rotation_y(const f32 radians) -> Mat4 {
  auto result = identity();

  result.m11 = result.m33 = std::cos(radians);
  result.m31 = std::sin(radians);
  result.m13 = -result.m31;

  return result;
}

auto Mat4::rotation_z(const f32 radians) -> Mat4 {
  auto result = identity();

  result.m11 = result.m22 = std::cos(radians);
  result.m12 = std::sin(radians);
  result.m21 = -result.m12;

  return result;
}

auto Mat4::rotation(const Vec3f32& radians) -> Mat4 {
  const auto x = rotation_x(radians.x);
  const auto y = rotation_y(radians.y);
  const auto z = rotation_z(radians.z);
  return z * x * y;
}

auto Mat4::perspective_projection(
  const f32 fovRadians, const f32 aspectRatio, const f32 nearPlane
, const f32 farPlane) -> Mat4 {
  const auto yScale = 1.0f / std::tan(fovRadians * 0.5f);
  const auto xScale = yScale / aspectRatio;
  const auto q = farPlane / (farPlane - nearPlane);

  return Mat4 {
    xScale,   0.0f,           0.0f, 0.0f,
      0.0f, yScale,           0.0f, 0.0f,
      0.0f,   0.0f,              q, 1.0f,
      0.0f,   0.0f, -q * nearPlane, 0.0f
  };
}

auto operator/(const Mat4& a, const Mat4& b) noexcept -> Mat4 {
  return a * Mat4::invert(b);
}

} // namespace basalt
