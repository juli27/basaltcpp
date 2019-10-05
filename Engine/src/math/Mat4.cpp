#include <basalt/math/Mat4.h>

#include <cmath>

namespace basalt::math {

auto Mat4::invert(const Mat4& m) -> Mat4 {
  auto invDet = m.det();
  if (invDet == 0.0f) {
    return identity();
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

auto Mat4::rotation_x(const f32 rad) -> Mat4 {
  auto result = identity();

  result.m22 = result.m33 = std::cosf(rad);
  result.m23 = std::sinf(rad);
  result.m32 = -result.m23;

  return result;
}

auto Mat4::rotation_y(const f32 rad) -> Mat4 {
  auto result = identity();

  result.m11 = result.m33 = std::cosf(rad);
  result.m31 = std::sinf(rad);
  result.m13 = -result.m31;

  return result;
}

auto Mat4::rotation_z(const f32 rad) -> Mat4 {
  auto result = identity();

  result.m11 = result.m22 = std::cosf(rad);
  result.m12 = std::sinf(rad);
  result.m21 = -result.m12;

  return result;
}

auto Mat4::rotation(const Vec3f32& xyzRad) -> Mat4 {
  const auto x = rotation_x(xyzRad.x());
  const auto y = rotation_y(xyzRad.y());
  const auto z = rotation_z(xyzRad.z());
  return z * x * y;
}

auto Mat4::perspective_projection(
  const f32 fovRad, const f32 aspectRatio, const f32 nearPlane, const f32 farPlane
) -> Mat4 {
  const auto yScale = 1.0f / std::tanf(fovRad * 0.5f);
  const auto xScale = yScale / aspectRatio;
  const auto q = farPlane / (farPlane - nearPlane);

  return Mat4(
    xScale, 0.0f  ,  0.0f         , 0.0f,
    0.0f  , yScale,  0.0f         , 0.0f,
    0.0f  , 0.0f  ,  q            , 1.0f,
    0.0f  , 0.0f  , -q * nearPlane, 0.0f
  );
}

} // namespace basalt::math
