#include "pch.h"

#include <cmath>

#include <basalt/math/Mat4.h>

namespace basalt::math {


auto Mat4::Invert(const Mat4& m) -> Mat4 {
  f32 invDet = m.Det();
  if (invDet == 0.0f) {
    return Mat4::Identity();
  }

  invDet = 1.0f / invDet;

  Mat4 result;
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


auto Mat4::Translation(const math::Vec3f32& v) -> Mat4 {
  return Mat4(
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    v.GetX() , v.GetY() , v.GetZ() , 1.0f
  );
}


auto Mat4::RotationX(f32 rad) -> Mat4 {
  Mat4 result = Mat4::Identity();

  result.m22 = result.m33 = std::cosf(rad);
  result.m23 = std::sinf(rad);
  result.m32 = -result.m23;

  return result;
}


auto Mat4::RotationY(f32 rad) -> Mat4 {
  Mat4 result = Mat4::Identity();

  result.m11 = result.m33 = std::cosf(rad);
  result.m31 = std::sinf(rad);
  result.m13 = -result.m31;

  return result;
}


auto Mat4::RotationZ(f32 rad) -> Mat4 {
  Mat4 result = Mat4::Identity();

  result.m11 = result.m22 = std::cosf(rad);
  result.m12 = std::sinf(rad);
  result.m21 = -result.m12;

  return result;
}


auto Mat4::Rotation(f32 xRad, f32 yRad, f32 zRad) -> Mat4 {
  return RotationZ(zRad) * RotationX(xRad) * RotationY(yRad);
}


auto Mat4::Rotation(const Vec3f32& xyzRad) -> Mat4 {
  return RotationZ(xyzRad.GetZ()) *
    RotationX(xyzRad.GetX()) *
    RotationY(xyzRad.GetY());
}

//Mat4 Mat4::RotationAxis(const tbVector3& axis, f32 rad) {
//  const f32 sin = std::sinf(-rad);
//  const f32 cos = std::cosf(-rad);
//  const f32 oneMinusCos = 1.0f - cos;
//
//  const tbVector3 axisNorm = tbVector3Normalize(axis);
//
//  return Mat4((axisNorm.x * axisNorm.x) * oneMinusCos + cos,
//                (axisNorm.x * axisNorm.y) * oneMinusCos - (axisNorm.z * sin),
//                (axisNorm.x * axisNorm.z) * oneMinusCos + (axisNorm.y * sin),
//                0.0f,
//
//                (axisNorm.y * axisNorm.x) * oneMinusCos + (axisNorm.z * sin),
//                (axisNorm.y * axisNorm.y) * oneMinusCos + cos,
//                (axisNorm.y * axisNorm.z) * oneMinusCos - (axisNorm.x * sin),
//                0.0f,
//
//                (axisNorm.z * axisNorm.x) * oneMinusCos - (axisNorm.y * sin),
//                (axisNorm.z * axisNorm.y) * oneMinusCos + (axisNorm.x * sin),
//                (axisNorm.z * axisNorm.z) * oneMinusCos + cos,
//                0.0f,
//
//                0.0f,
//                0.0f,
//                0.0f,
//                1.0f);
//}
//
auto Mat4::Scaling(const Vec3f32& v) -> Mat4 {
  return Mat4(
    v.GetX(), 0.0f    , 0.0f    , 0.0f,
    0.0f    , v.GetY(), 0.0f    , 0.0f,
    0.0f    , 0.0f    , v.GetZ(), 0.0f,
    0.0f    , 0.0f    , 0.0f    , 1.0f
  );
}

//Mat4 Mat4::Axes(const tbVector3& xAxis, const tbVector3& yAxis,
//                      const tbVector3& zAxis) {
//  return Mat4(xAxis.x, xAxis.y, xAxis.z, 0.0f,
//                yAxis.x, yAxis.y, yAxis.z, 0.0f,
//                zAxis.x, zAxis.y, zAxis.z, 0.0f,
//                0.0f   , 0.0f   , 0.0f   , 1.0f);
//}

//Mat4 Mat4::Transpose(const Mat4& m) {
//  return Mat4(m.m11, m.m21, m.m31, m.m41,
//                m.m12, m.m22, m.m32, m.m42,
//                m.m13, m.m23, m.m33, m.m43,
//                m.m14, m.m24, m.m34, m.m44);
//}


auto Mat4::PerspectiveProjection(
  f32 fovRad, f32 aspectRatio, f32 nearPlane, f32 farPlane
) -> Mat4 {
  const f32 yScale = 1.0f / std::tanf(fovRad * 0.5f);
  const f32 xScale = yScale / aspectRatio;
  const f32 Q = farPlane / (farPlane - nearPlane);

  return Mat4(
    xScale, 0.0f  ,  0.0f         , 0.0f,
    0.0f  , yScale,  0.0f         , 0.0f,
    0.0f  , 0.0f  ,  Q            , 1.0f,
    0.0f  , 0.0f  , -Q * nearPlane, 0.0f
  );
}


//Mat4 Mat4::ToTex2DMatrix(const Mat4& m) {
//  return Mat4(m.m11, m.m12, m.m14, 0.0f,
//                m.m21, m.m22, m.m24, 0.0f,
//                m.m41, m.m42, m.m44, 0.0f,
//                0.0f , 0.0f , 0.0f , 1.0f);
//}

} // namespace basalt::math
