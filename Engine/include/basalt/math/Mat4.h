#pragma once
#ifndef BS_MATH_MAT4_H
#define BS_MATH_MAT4_H

#include "Vec3.h"

namespace basalt {
namespace math {


class Mat4 final {
public:
  f32 m11, m12, m13, m14,
      m21, m22, m23, m24,
      m31, m32, m33, m34,
      m41, m42, m43, m44;

  constexpr Mat4();

  constexpr Mat4(
    f32 _m11, f32 _m12, f32 _m13, f32 _m14,
    f32 _m21, f32 _m22, f32 _m23, f32 _m24,
    f32 _m31, f32 _m32, f32 _m33, f32 _m34,
    f32 _m41, f32 _m42, f32 _m43, f32 _m44
  );

  constexpr Mat4(const Mat4&) = default;

  constexpr Mat4(Mat4&&) = default;

  inline ~Mat4() = default;

  constexpr f32 Det() const;

  inline Mat4& operator=(const Mat4&) = default;

  inline Mat4& operator=(Mat4&&) = default;

  constexpr Mat4& operator+=(const Mat4& m);

  constexpr Mat4& operator-=(const Mat4& m);

  constexpr Mat4& operator*=(const Mat4& m);

  constexpr Mat4& operator*=(f32 f);

  inline Mat4& operator/=(const Mat4& m);

  constexpr Mat4& operator/=(f32 f);

  static constexpr Mat4 Identity();

  static Mat4 Invert(const Mat4& m);

  static Mat4 Translation(const Vec3f32& v);

  static Mat4 RotationX(f32 rad);

  static Mat4 RotationY(f32 rad);

  static Mat4 RotationZ(f32 rad);

  static Mat4 Rotation(f32 xRad, f32 yRad, f32 zRad);

  //static Mat4 Rotation(const tbVector3& xyzRad);

  //static Mat4 RotationAxis(const tbVector3& axis, f32 rad);

  //static Mat4 Scaling(const tbVector3& v);

  /*static Mat4 Axes(const tbVector3& xAxis, const tbVector3& yAxis,
                     const tbVector3& zAxis);*/

  //static Mat4 Transpose(const Mat4& m);

  static Mat4 PerspectiveProjection(
    f32 fovRad, f32 aspectRatio, f32 nearPlane, f32 farPlane
  );

  static Mat4 Camera(
    const Vec3f32& pos, const Vec3f32& lookAt, const Vec3f32& up
  );

  //static Mat4 ToTex2DMatrix(const Mat4& m);
};


constexpr Mat4::Mat4()
  : m11(0.0f), m12(0.0f), m13(0.0f), m14(0.0f),
    m21(0.0f), m22(0.0f), m23(0.0f), m24(0.0f),
    m31(0.0f), m32(0.0f), m33(0.0f), m34(0.0f),
    m41(0.0f), m42(0.0f), m43(0.0f), m44(0.0f) {}


constexpr Mat4::Mat4(
  f32 _m11, f32 _m12, f32 _m13, f32 _m14,
  f32 _m21, f32 _m22, f32 _m23, f32 _m24,
  f32 _m31, f32 _m32, f32 _m33, f32 _m34,
  f32 _m41, f32 _m42, f32 _m43, f32 _m44
) : m11(_m11), m12(_m12), m13(_m13), m14(_m14),
    m21(_m21), m22(_m22), m23(_m23), m24(_m24),
    m31(_m31), m32(_m32), m33(_m33), m34(_m34),
    m41(_m41), m42(_m42), m43(_m43), m44(_m44) {}


// Determinante der linken oberen 3x3-Teilmatrix berechnen
constexpr f32 Mat4::Det() const {
  return m11 * (m22 * m33 - m23 * m32)
    - m12 * (m21 * m33 - m23 * m31)
    + m13 * (m21 * m32 - m22 * m31);
}


constexpr Mat4& Mat4::operator+=(const Mat4& m) {
  m11 += m.m11; m12 += m.m12; m13 += m.m13; m14 += m.m14;
  m21 += m.m21; m22 += m.m22; m23 += m.m23; m24 += m.m24;
  m31 += m.m31; m32 += m.m32; m33 += m.m33; m34 += m.m34;
  m41 += m.m41; m42 += m.m42; m43 += m.m43; m44 += m.m44;

  return *this;
}


constexpr Mat4& Mat4::operator-=(const Mat4& m) {
  m11 -= m.m11; m12 -= m.m12; m13 -= m.m13; m14 -= m.m14;
  m21 -= m.m21; m22 -= m.m22; m23 -= m.m23; m24 -= m.m24;
  m31 -= m.m31; m32 -= m.m32; m33 -= m.m33; m34 -= m.m34;
  m41 -= m.m41; m42 -= m.m42; m43 -= m.m43; m44 -= m.m44;

  return *this;
}


constexpr Mat4& Mat4::operator*=(const Mat4& m) {
  *this = Mat4(
    m.m11 * m11 + m.m21 * m12 + m.m31 * m13 + m.m41 * m14,
    m.m12 * m11 + m.m22 * m12 + m.m32 * m13 + m.m42 * m14,
    m.m13 * m11 + m.m23 * m12 + m.m33 * m13 + m.m43 * m14,
    m.m14 * m11 + m.m24 * m12 + m.m34 * m13 + m.m44 * m14,

    m.m11 * m21 + m.m21 * m22 + m.m31 * m23 + m.m41 * m24,
    m.m12 * m21 + m.m22 * m22 + m.m32 * m23 + m.m42 * m24,
    m.m13 * m21 + m.m23 * m22 + m.m33 * m23 + m.m43 * m24,
    m.m14 * m21 + m.m24 * m22 + m.m34 * m23 + m.m44 * m24,

    m.m11 * m31 + m.m21 * m32 + m.m31 * m33 + m.m41 * m34,
    m.m12 * m31 + m.m22 * m32 + m.m32 * m33 + m.m42 * m34,
    m.m13 * m31 + m.m23 * m32 + m.m33 * m33 + m.m43 * m34,
    m.m14 * m31 + m.m24 * m32 + m.m34 * m33 + m.m44 * m34,

    m.m11 * m41 + m.m21 * m42 + m.m31 * m43 + m.m41 * m44,
    m.m12 * m41 + m.m22 * m42 + m.m32 * m43 + m.m42 * m44,
    m.m13 * m41 + m.m23 * m42 + m.m33 * m43 + m.m43 * m44,
    m.m14 * m41 + m.m24 * m42 + m.m34 * m43 + m.m44 * m44
  );

  return *this;
}


constexpr Mat4& Mat4::operator*=(f32 f) {
  m11 *= f; m12 *= f; m13 *= f; m14 *= f;
  m21 *= f; m22 *= f; m23 *= f; m24 *= f;
  m31 *= f; m32 *= f; m33 *= f; m34 *= f;
  m41 *= f; m42 *= f; m43 *= f; m44 *= f;

  return *this;
}


inline Mat4& Mat4::operator/=(const Mat4& m) {
  *this *= Mat4::Invert(m);
  return *this;
}


constexpr Mat4& Mat4::operator/=(f32 f) {
  m11 /= f; m12 /= f; m13 /= f; m14 /= f;
  m21 /= f; m22 /= f; m23 /= f; m24 /= f;
  m31 /= f; m32 /= f; m33 /= f; m34 /= f;
  m41 /= f; m42 /= f; m43 /= f; m44 /= f;

  return *this;
}


constexpr Mat4 Mat4::Identity() {
  return Mat4(
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  );
}


// global operators


constexpr Mat4 operator-(const Mat4& m) {
  return Mat4(
    -m.m11, -m.m12, -m.m13, -m.m14,
    -m.m21, -m.m22, -m.m23, -m.m24,
    -m.m31, -m.m32, -m.m33, -m.m34,
    -m.m41, -m.m42, -m.m43, -m.m44
  );
}


constexpr Mat4 operator+(const Mat4& a, const Mat4& b) {
  return Mat4(
    a.m11 + b.m11, a.m12 + b.m12, a.m13 + b.m13, a.m14 + b.m14,
    a.m21 + b.m21, a.m22 + b.m22, a.m23 + b.m23, a.m24 + b.m24,
    a.m31 + b.m31, a.m32 + b.m32, a.m33 + b.m33, a.m34 + b.m34,
    a.m41 + b.m41, a.m42 + b.m42, a.m43 + b.m43, a.m44 + b.m44
  );
}


constexpr Mat4 operator-(const Mat4& a, const Mat4& b) {
  return Mat4(
    a.m11 - b.m11, a.m12 - b.m12, a.m13 - b.m13, a.m14 - b.m14,
    a.m21 - b.m21, a.m22 - b.m22, a.m23 - b.m23, a.m24 - b.m24,
    a.m31 - b.m31, a.m32 - b.m32, a.m33 - b.m33, a.m34 - b.m34,
    a.m41 - b.m41, a.m42 - b.m42, a.m43 - b.m43, a.m44 - b.m44
  );
}


constexpr Mat4 operator*(const Mat4& a, const Mat4& b) {
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


constexpr Mat4 operator*(const Mat4& m, f32 f) {
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


inline Mat4 operator/(const Mat4& a, const Mat4& b) {
  return a * Mat4::Invert(b);
}


constexpr Mat4 operator/(const Mat4& m, f32 f) {
  return Mat4(
    m.m11 / f, m.m12 / f, m.m13 / f, m.m14 / f,
    m.m21 / f, m.m22 / f, m.m23 / f, m.m24 / f,
    m.m31 / f, m.m32 / f, m.m33 / f, m.m34 / f,
    m.m41 / f, m.m42 / f, m.m43 / f, m.m44 / f
  );
}


constexpr bool operator==(const Mat4& a, const Mat4& b) {
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


constexpr bool operator!=(const Mat4& a, const Mat4& b) {
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

} // namespace math
} // namespace basalt

#endif // !BS_MATH_MAT4_H
