#pragma once
#ifndef BASALT_MATH_MAT4_H
#define BASALT_MATH_MAT4_H

#include "runtime/math/Vec3.h"
#include "runtime/shared/Types.h"

namespace basalt::math {

struct Mat4 final {
  constexpr Mat4() noexcept = default;

  constexpr Mat4(
    f32 m11, f32 m12, f32 m13, f32 m14
  , f32 m21, f32 m22, f32 m23, f32 m24
  , f32 m31, f32 m32, f32 m33, f32 m34
  , f32 m41, f32 m42, f32 m43, f32 m44
  ) noexcept;

  constexpr Mat4(const Mat4&) noexcept = default;
  constexpr Mat4(Mat4&&) noexcept = default;

  ~Mat4() noexcept = default;

  auto operator=(const Mat4&) noexcept -> Mat4& = default;
  auto operator=(Mat4&&) noexcept -> Mat4& = default;

  constexpr auto operator+=(const Mat4& rhs) noexcept -> Mat4&;
  constexpr auto operator-=(const Mat4& rhs) noexcept -> Mat4&;
  constexpr auto operator*=(const Mat4& rhs) noexcept -> Mat4&;
  constexpr auto operator*=(f32 rhs) noexcept -> Mat4&;
  inline auto operator/=(const Mat4& rhs) noexcept -> Mat4&;
  constexpr auto operator/=(f32 rhs) noexcept -> Mat4&;

  [[nodiscard]]
  constexpr auto det() const noexcept -> f32;

  f32 m11 = 0.0f, m12 = 0.0f, m13 = 0.0f, m14 = 0.0f,
      m21 = 0.0f, m22 = 0.0f, m23 = 0.0f, m24 = 0.0f,
      m31 = 0.0f, m32 = 0.0f, m33 = 0.0f, m34 = 0.0f,
      m41 = 0.0f, m42 = 0.0f, m43 = 0.0f, m44 = 0.0f;

  static constexpr auto identity() noexcept -> Mat4;
  static auto invert(const Mat4& m) -> Mat4;
  static constexpr auto translation(const Vec3f32& v) -> Mat4;
  static auto rotation_x(f32 rad) -> Mat4;
  static auto rotation_y(f32 rad) -> Mat4;
  static auto rotation_z(f32 rad) -> Mat4;
  static auto rotation(const Vec3f32& xyzRad) -> Mat4;
  static constexpr auto scaling(const Vec3f32& v) -> Mat4;
  static auto perspective_projection(
    f32 fovRad, f32 aspectRatio, f32 nearPlane, f32 farPlane
  ) -> Mat4;
};

constexpr Mat4::Mat4(
  const f32 m11, const f32 m12, const f32 m13, const f32 m14,
  const f32 m21, const f32 m22, const f32 m23, const f32 m24,
  const f32 m31, const f32 m32, const f32 m33, const f32 m34,
  const f32 m41, const f32 m42, const f32 m43, const f32 m44
) noexcept
: m11(m11), m12(m12), m13(m13), m14(m14)
, m21(m21), m22(m22), m23(m23), m24(m24)
, m31(m31), m32(m32), m33(m33), m34(m34)
, m41(m41), m42(m42), m43(m43), m44(m44) {}

constexpr auto Mat4::operator+=(const Mat4& rhs) noexcept -> Mat4& {
  m11 += rhs.m11; m12 += rhs.m12; m13 += rhs.m13; m14 += rhs.m14;
  m21 += rhs.m21; m22 += rhs.m22; m23 += rhs.m23; m24 += rhs.m24;
  m31 += rhs.m31; m32 += rhs.m32; m33 += rhs.m33; m34 += rhs.m34;
  m41 += rhs.m41; m42 += rhs.m42; m43 += rhs.m43; m44 += rhs.m44;

  return *this;
}

constexpr auto Mat4::operator-=(const Mat4& rhs) noexcept -> Mat4& {
  m11 -= rhs.m11; m12 -= rhs.m12; m13 -= rhs.m13; m14 -= rhs.m14;
  m21 -= rhs.m21; m22 -= rhs.m22; m23 -= rhs.m23; m24 -= rhs.m24;
  m31 -= rhs.m31; m32 -= rhs.m32; m33 -= rhs.m33; m34 -= rhs.m34;
  m41 -= rhs.m41; m42 -= rhs.m42; m43 -= rhs.m43; m44 -= rhs.m44;

  return *this;
}

constexpr auto Mat4::operator*=(const Mat4& rhs) noexcept -> Mat4& {
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

constexpr auto Mat4::operator*=(const f32 rhs) noexcept -> Mat4& {
  m11 *= rhs; m12 *= rhs; m13 *= rhs; m14 *= rhs;
  m21 *= rhs; m22 *= rhs; m23 *= rhs; m24 *= rhs;
  m31 *= rhs; m32 *= rhs; m33 *= rhs; m34 *= rhs;
  m41 *= rhs; m42 *= rhs; m43 *= rhs; m44 *= rhs;

  return *this;
}

inline auto Mat4::operator/=(const Mat4& rhs) noexcept -> Mat4& {
  *this *= invert(rhs);
  return *this;
}

constexpr auto Mat4::operator/=(const f32 rhs) noexcept -> Mat4& {
  m11 /= rhs; m12 /= rhs; m13 /= rhs; m14 /= rhs;
  m21 /= rhs; m22 /= rhs; m23 /= rhs; m24 /= rhs;
  m31 /= rhs; m32 /= rhs; m33 /= rhs; m34 /= rhs;
  m41 /= rhs; m42 /= rhs; m43 /= rhs; m44 /= rhs;

  return *this;
}

// Determinante der linken oberen 3x3-Teilmatrix berechnen
constexpr auto Mat4::det() const noexcept -> f32 {
  return m11 * (m22 * m33 - m23 * m32)
       - m12 * (m21 * m33 - m23 * m31)
       + m13 * (m21 * m32 - m22 * m31);
}

constexpr auto Mat4::identity() noexcept -> Mat4 {
  return Mat4(
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  );
}

constexpr auto Mat4::translation(const math::Vec3f32& v) -> Mat4 {
  return Mat4(
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    v.x , v.y , v.z , 1.0f
  );
}

constexpr auto Mat4::scaling(const Vec3f32& v) -> Mat4 {
  return Mat4(
    v.x, 0.0f    , 0.0f    , 0.0f,
    0.0f    , v.y, 0.0f    , 0.0f,
    0.0f    , 0.0f    , v.z, 0.0f,
    0.0f    , 0.0f    , 0.0f    , 1.0f
  );
}

constexpr auto operator-(const Mat4& m) -> Mat4 {
  return Mat4(
    -m.m11, -m.m12, -m.m13, -m.m14,
    -m.m21, -m.m22, -m.m23, -m.m24,
    -m.m31, -m.m32, -m.m33, -m.m34,
    -m.m41, -m.m42, -m.m43, -m.m44
  );
}

constexpr auto operator+(const Mat4& a, const Mat4& b) -> Mat4 {
  return Mat4(
    a.m11 + b.m11, a.m12 + b.m12, a.m13 + b.m13, a.m14 + b.m14,
    a.m21 + b.m21, a.m22 + b.m22, a.m23 + b.m23, a.m24 + b.m24,
    a.m31 + b.m31, a.m32 + b.m32, a.m33 + b.m33, a.m34 + b.m34,
    a.m41 + b.m41, a.m42 + b.m42, a.m43 + b.m43, a.m44 + b.m44
  );
}

constexpr auto operator-(const Mat4& a, const Mat4& b) -> Mat4 {
  return Mat4(
    a.m11 - b.m11, a.m12 - b.m12, a.m13 - b.m13, a.m14 - b.m14,
    a.m21 - b.m21, a.m22 - b.m22, a.m23 - b.m23, a.m24 - b.m24,
    a.m31 - b.m31, a.m32 - b.m32, a.m33 - b.m33, a.m34 - b.m34,
    a.m41 - b.m41, a.m42 - b.m42, a.m43 - b.m43, a.m44 - b.m44
  );
}

constexpr auto operator*(const Mat4& a, const Mat4& b) -> Mat4 {
  return Mat4(
    b.m11 * a.m11 + b.m21 * a.m12 + b.m31 * a.m13 + b.m41 * a.m14,
    b.m12 * a.m11 + b.m22 * a.m12 + b.m32 * a.m13 + b.m42 * a.m14,
    b.m13 * a.m11 + b.m23 * a.m12 + b.m33 * a.m13 + b.m43 * a.m14,
    b.m14 * a.m11 + b.m24 * a.m12 + b.m34 * a.m13 + b.m44 * a.m14,

    b.m11 * a.m21 + b.m21 * a.m22 + b.m31 * a.m23 + b.m41 * a.m24,
    b.m12 * a.m21 + b.m22 * a.m22 + b.m32 * a.m23 + b.m42 * a.m24,
    b.m13 * a.m21 + b.m23 * a.m22 + b.m33 * a.m23 + b.m43 * a.m24,
    b.m14 * a.m21 + b.m24 * a.m22 + b.m34 * a.m23 + b.m44 * a.m24,

    b.m11 * a.m31 + b.m21 * a.m32 + b.m31 * a.m33 + b.m41 * a.m34,
    b.m12 * a.m31 + b.m22 * a.m32 + b.m32 * a.m33 + b.m42 * a.m34,
    b.m13 * a.m31 + b.m23 * a.m32 + b.m33 * a.m33 + b.m43 * a.m34,
    b.m14 * a.m31 + b.m24 * a.m32 + b.m34 * a.m33 + b.m44 * a.m34,

    b.m11 * a.m41 + b.m21 * a.m42 + b.m31 * a.m43 + b.m41 * a.m44,
    b.m12 * a.m41 + b.m22 * a.m42 + b.m32 * a.m43 + b.m42 * a.m44,
    b.m13 * a.m41 + b.m23 * a.m42 + b.m33 * a.m43 + b.m43 * a.m44,
    b.m14 * a.m41 + b.m24 * a.m42 + b.m34 * a.m43 + b.m44 * a.m44
  );
}

constexpr auto operator*(const Mat4& m, f32 f) -> Mat4 {
  return Mat4(
    m.m11 * f, m.m12 * f, m.m13 * f, m.m14 * f,
    m.m21 * f, m.m22 * f, m.m23 * f, m.m24 * f,
    m.m31 * f, m.m32 * f, m.m33 * f, m.m34 * f,
    m.m41 * f, m.m42 * f, m.m43 * f, m.m44 * f
  );
}

constexpr Mat4 operator*(f32 f, const Mat4& m) {
  return Mat4(
    m.m11 * f, m.m12 * f, m.m13 * f, m.m14 * f,
    m.m21 * f, m.m22 * f, m.m23 * f, m.m24 * f,
    m.m31 * f, m.m32 * f, m.m33 * f, m.m34 * f,
    m.m41 * f, m.m42 * f, m.m43 * f, m.m44 * f
  );
}

inline auto operator/(const Mat4& a, const Mat4& b) -> Mat4 {
  return a * Mat4::invert(b);
}

constexpr auto operator/(const Mat4& m, f32 f) -> Mat4 {
  return Mat4(
    m.m11 / f, m.m12 / f, m.m13 / f, m.m14 / f,
    m.m21 / f, m.m22 / f, m.m23 / f, m.m24 / f,
    m.m31 / f, m.m32 / f, m.m33 / f, m.m34 / f,
    m.m41 / f, m.m42 / f, m.m43 / f, m.m44 / f
  );
}

constexpr auto operator==(const Mat4& a, const Mat4& b) -> bool {
  if (a.m11 != b.m11) return false;
  if (a.m12 != b.m12) return false;
  if (a.m13 != b.m13) return false;
  if (a.m14 != b.m14) return false;
  if (a.m21 != b.m21) return false;
  if (a.m22 != b.m22) return false;
  if (a.m23 != b.m23) return false;
  if (a.m24 != b.m24) return false;
  if (a.m31 != b.m31) return false;
  if (a.m32 != b.m32) return false;
  if (a.m33 != b.m33) return false;
  if (a.m34 != b.m34) return false;
  if (a.m41 != b.m41) return false;
  if (a.m42 != b.m42) return false;
  if (a.m43 != b.m43) return false;

  return a.m44 == b.m44;
}

constexpr auto operator!=(const Mat4& a, const Mat4& b) -> bool {
  if (a.m11 != b.m11) return true;
  if (a.m12 != b.m12) return true;
  if (a.m13 != b.m13) return true;
  if (a.m14 != b.m14) return true;
  if (a.m21 != b.m21) return true;
  if (a.m22 != b.m22) return true;
  if (a.m23 != b.m23) return true;
  if (a.m24 != b.m24) return true;
  if (a.m31 != b.m31) return true;
  if (a.m32 != b.m32) return true;
  if (a.m33 != b.m33) return true;
  if (a.m34 != b.m34) return true;
  if (a.m41 != b.m41) return true;
  if (a.m42 != b.m42) return true;
  if (a.m43 != b.m43) return true;

  return a.m44 != b.m44;
}

using Mat4f32 = Mat4;

} // namespace basalt::math

#endif // !BASALT_MATH_MAT4_H
