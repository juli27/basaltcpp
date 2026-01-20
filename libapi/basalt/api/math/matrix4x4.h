#pragma once

#include <basalt/api/math/types.h>
#include <basalt/api/math/vector3.h>

#include <basalt/api/base/types.h>

namespace basalt {

class Matrix4x4f32 final {
public:
  f32 m11{}, m12{}, m13{}, m14{};
  f32 m21{}, m22{}, m23{}, m24{};
  f32 m31{}, m32{}, m33{}, m34{};
  f32 m41{}, m42{}, m43{}, m44{};

  [[nodiscard]]
  static constexpr auto zero() noexcept -> Matrix4x4f32 {
    return Matrix4x4f32{};
  }

  [[nodiscard]]
  static constexpr auto identity() noexcept -> Matrix4x4f32 {
    // clang-format off
    return Matrix4x4f32{
      1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f,
    };
    // clang-format on
  }

  [[nodiscard]]
  static constexpr auto translation(f32 const x, f32 const y,
                                    f32 const z) noexcept -> Matrix4x4f32 {
    // clang-format off
    return Matrix4x4f32{
      1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f,
         x,    y,    z, 1.0f,
    };
    // clang-format on
  }

  [[nodiscard]]
  static constexpr auto translation(Vector3f32 const& t) noexcept
    -> Matrix4x4f32 {
    return translation(t.x(), t.y(), t.z());
  }

  [[nodiscard]] static auto rotation_x(Angle) noexcept -> Matrix4x4f32;

  [[nodiscard]] static auto rotation_y(Angle) noexcept -> Matrix4x4f32;

  [[nodiscard]] static auto rotation_z(Angle) noexcept -> Matrix4x4f32;

  [[nodiscard]]
  static auto rotation(Angle x, Angle y, Angle z) noexcept -> Matrix4x4f32;

  [[nodiscard]]
  static auto rotation(Vector3f32 const& radians) noexcept -> Matrix4x4f32;

  [[nodiscard]]
  static auto rotation(Vector3f32 axis, Angle) noexcept -> Matrix4x4f32;

  [[nodiscard]]
  static constexpr auto scaling(f32 const s) noexcept -> Matrix4x4f32 {
    return scaling(Vector3f32{s});
  }

  [[nodiscard]]
  static constexpr auto scaling(f32 const x, f32 const y, f32 const z) noexcept
    -> Matrix4x4f32 {
    // clang-format off
    return Matrix4x4f32{
         x, 0.0f, 0.0f, 0.0f,
      0.0f,    y, 0.0f, 0.0f,
      0.0f, 0.0f,    z, 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f,
    };
    // clang-format on
  }

  [[nodiscard]]
  static constexpr auto scaling(Vector3f32 const& s) noexcept -> Matrix4x4f32 {
    return scaling(s.x(), s.y(), s.z());
  }

  [[nodiscard]]
  static constexpr auto axes(Vector3f32 const& x, Vector3f32 const& y,
                             Vector3f32 const& z) noexcept -> Matrix4x4f32 {
    // clang-format off
    return Matrix4x4f32{
      x.x(), x.y(), x.z(), 0.0f,
      y.x(), y.y(), y.z(), 0.0f,
      z.x(), z.y(), z.z(), 0.0f,
      0.0f,  0.0f,  0.0f,  1.0f,
    };
    // clang-format on
  }

  [[nodiscard]]
  static constexpr auto inverted(Matrix4x4f32 const& m) noexcept
    -> Matrix4x4f32 {
    auto const det = m.det_3x3();
    if (det == 0.0f) {
      return identity();
    }

    auto const invDet = 1.0f / det;

    auto result = Matrix4x4f32{
      invDet * (m.m22 * m.m33 - m.m23 * m.m32),
      -invDet * (m.m12 * m.m33 - m.m13 * m.m32),
      invDet * (m.m12 * m.m23 - m.m13 * m.m22),
      0.0f,
      -invDet * (m.m21 * m.m33 - m.m23 * m.m31),
      invDet * (m.m11 * m.m33 - m.m13 * m.m31),
      -invDet * (m.m11 * m.m23 - m.m13 * m.m21),
      0.0f,
      invDet * (m.m21 * m.m32 - m.m22 * m.m31),
      -invDet * (m.m11 * m.m32 - m.m12 * m.m31),
      invDet * (m.m11 * m.m22 - m.m12 * m.m21),
      0.0f,
      0.0f,
      0.0f,
      0.0f,
      1.0f,
    };

    result.m41 =
      -(m.m41 * result.m11 + m.m42 * result.m21 + m.m43 * result.m31);
    result.m42 =
      -(m.m41 * result.m12 + m.m42 * result.m22 + m.m43 * result.m32);
    result.m43 =
      -(m.m41 * result.m13 + m.m42 * result.m23 + m.m43 * result.m33);

    return result;
  }

  [[nodiscard]]
  static constexpr auto transposed(Matrix4x4f32 const& m) noexcept
    -> Matrix4x4f32 {
    // clang-format off
    return Matrix4x4f32{
      m.m11, m.m21, m.m31, m.m41,
      m.m12, m.m22, m.m32, m.m42,
      m.m13, m.m23, m.m33, m.m43,
      m.m14, m.m24, m.m34, m.m44,
    };
    // clang-format on
  }

  [[nodiscard]]
  static auto look_at_lh(Vector3f32 const& position, Vector3f32 const& lookAt,
                         Vector3f32 const& up) -> Matrix4x4f32;

  [[nodiscard]]
  static auto perspective_projection(Angle fov, f32 aspectRatio, f32 nearPlaneZ,
                                     f32 farPlaneZ) noexcept -> Matrix4x4f32;

  constexpr Matrix4x4f32() noexcept = default;

  // clang-format off
  constexpr Matrix4x4f32(
     f32 const m11,  f32 const m12,  f32 const m13,  f32 const m14,
     f32 const m21,  f32 const m22,  f32 const m23,  f32 const m24,
     f32 const m31,  f32 const m32,  f32 const m33,  f32 const m34,
     f32 const m41,  f32 const m42,  f32 const m43,  f32 const m44) noexcept
    : m11{m11}, m12{m12}, m13{m13}, m14{m14}
    , m21{m21}, m22{m22}, m23{m23}, m24{m24}
    , m31{m31}, m32{m32}, m33{m33}, m34{m34}
    , m41{m41}, m42{m42}, m43{m43}, m44{m44} {
  }

  // clang-format on

  constexpr auto operator==(Matrix4x4f32 const& r) const noexcept -> bool {
    return m11 == r.m11 && m12 == r.m12 && m13 == r.m13 && m14 == r.m14 &&
           m21 == r.m21 && m22 == r.m22 && m23 == r.m23 && m24 == r.m24 &&
           m31 == r.m31 && m32 == r.m32 && m33 == r.m33 && m34 == r.m34 &&
           m41 == r.m41 && m42 == r.m42 && m43 == r.m43 && m44 == r.m44;
  }

  constexpr auto operator!=(Matrix4x4f32 const& r) const noexcept -> bool {
    return !(*this == r);
  }

  constexpr auto operator+=(Matrix4x4f32 const& rhs) noexcept -> Matrix4x4f32& {
    // clang-format off
    m11 += rhs.m11; m12 += rhs.m12; m13 += rhs.m13; m14 += rhs.m14;
    m21 += rhs.m21; m22 += rhs.m22; m23 += rhs.m23; m24 += rhs.m24;
    m31 += rhs.m31; m32 += rhs.m32; m33 += rhs.m33; m34 += rhs.m34;
    m41 += rhs.m41; m42 += rhs.m42; m43 += rhs.m43; m44 += rhs.m44;
    // clang-format on

    return *this;
  }

  constexpr auto operator-=(Matrix4x4f32 const& rhs) noexcept -> Matrix4x4f32& {
    // clang-format off
    m11 -= rhs.m11; m12 -= rhs.m12; m13 -= rhs.m13; m14 -= rhs.m14;
    m21 -= rhs.m21; m22 -= rhs.m22; m23 -= rhs.m23; m24 -= rhs.m24;
    m31 -= rhs.m31; m32 -= rhs.m32; m33 -= rhs.m33; m34 -= rhs.m34;
    m41 -= rhs.m41; m42 -= rhs.m42; m43 -= rhs.m43; m44 -= rhs.m44;
    // clang-format on

    return *this;
  }

  constexpr auto operator*=(Matrix4x4f32 const& rhs) noexcept -> Matrix4x4f32& {
    auto const lhs = *this;

    m11 = lhs.m11 * rhs.m11 + lhs.m12 * rhs.m21 + lhs.m13 * rhs.m31 +
          lhs.m14 * rhs.m41;
    m12 = lhs.m11 * rhs.m12 + lhs.m12 * rhs.m22 + lhs.m13 * rhs.m32 +
          lhs.m14 * rhs.m42;
    m13 = lhs.m11 * rhs.m13 + lhs.m12 * rhs.m23 + lhs.m13 * rhs.m33 +
          lhs.m14 * rhs.m43;
    m14 = lhs.m11 * rhs.m14 + lhs.m12 * rhs.m24 + lhs.m13 * rhs.m34 +
          lhs.m14 * rhs.m44;

    m21 = lhs.m21 * rhs.m11 + lhs.m22 * rhs.m21 + lhs.m23 * rhs.m31 +
          lhs.m24 * rhs.m41;
    m22 = lhs.m21 * rhs.m12 + lhs.m22 * rhs.m22 + lhs.m23 * rhs.m32 +
          lhs.m24 * rhs.m42;
    m23 = lhs.m21 * rhs.m13 + lhs.m22 * rhs.m23 + lhs.m23 * rhs.m33 +
          lhs.m24 * rhs.m43;
    m24 = lhs.m21 * rhs.m14 + lhs.m22 * rhs.m24 + lhs.m23 * rhs.m34 +
          lhs.m24 * rhs.m44;

    m31 = lhs.m31 * rhs.m11 + lhs.m32 * rhs.m21 + lhs.m33 * rhs.m31 +
          lhs.m34 * rhs.m41;
    m32 = lhs.m31 * rhs.m12 + lhs.m32 * rhs.m22 + lhs.m33 * rhs.m32 +
          lhs.m34 * rhs.m42;
    m33 = lhs.m31 * rhs.m13 + lhs.m32 * rhs.m23 + lhs.m33 * rhs.m33 +
          lhs.m34 * rhs.m43;
    m34 = lhs.m31 * rhs.m14 + lhs.m32 * rhs.m24 + lhs.m33 * rhs.m34 +
          lhs.m34 * rhs.m44;

    m41 = lhs.m41 * rhs.m11 + lhs.m42 * rhs.m21 + lhs.m43 * rhs.m31 +
          lhs.m44 * rhs.m41;
    m42 = lhs.m41 * rhs.m12 + lhs.m42 * rhs.m22 + lhs.m43 * rhs.m32 +
          lhs.m44 * rhs.m42;
    m43 = lhs.m41 * rhs.m13 + lhs.m42 * rhs.m23 + lhs.m43 * rhs.m33 +
          lhs.m44 * rhs.m43;
    m44 = lhs.m41 * rhs.m14 + lhs.m42 * rhs.m24 + lhs.m43 * rhs.m34 +
          lhs.m44 * rhs.m44;

    return *this;
  }

  constexpr auto operator*=(f32 const rhs) noexcept -> Matrix4x4f32& {
    // clang-format off
    m11 *= rhs; m12 *= rhs; m13 *= rhs; m14 *= rhs;
    m21 *= rhs; m22 *= rhs; m23 *= rhs; m24 *= rhs;
    m31 *= rhs; m32 *= rhs; m33 *= rhs; m34 *= rhs;
    m41 *= rhs; m42 *= rhs; m43 *= rhs; m44 *= rhs;
    // clang-format on

    return *this;
  }

  constexpr auto operator/=(Matrix4x4f32 const& rhs) noexcept -> Matrix4x4f32& {
    return *this *= inverted(rhs);
  }

  constexpr auto operator/=(f32 const rhs) noexcept -> Matrix4x4f32& {
    // clang-format off
    m11 /= rhs; m12 /= rhs; m13 /= rhs; m14 /= rhs;
    m21 /= rhs; m22 /= rhs; m23 /= rhs; m24 /= rhs;
    m31 /= rhs; m32 /= rhs; m33 /= rhs; m34 /= rhs;
    m41 /= rhs; m42 /= rhs; m43 /= rhs; m44 /= rhs;
    // clang-format on

    return *this;
  }

  [[nodiscard]] constexpr auto operator-() const noexcept -> Matrix4x4f32 {
    // clang-format off
    return Matrix4x4f32{
      -m11, -m12, -m13, -m14,
      -m21, -m22, -m23, -m24,
      -m31, -m32, -m33, -m34,
      -m41, -m42, -m43, -m44,
    };
    // clang-format on
  }

  [[nodiscard]] friend constexpr auto operator+(Matrix4x4f32 l,
                                                Matrix4x4f32 const& r) noexcept
    -> Matrix4x4f32 {
    return l += r;
  }

  [[nodiscard]] friend constexpr auto operator-(Matrix4x4f32 l,
                                                Matrix4x4f32 const& r) noexcept
    -> Matrix4x4f32 {
    return l -= r;
  }

  [[nodiscard]] friend constexpr auto operator*(Matrix4x4f32 l,
                                                Matrix4x4f32 const& r) noexcept
    -> Matrix4x4f32 {
    return l *= r;
  }

  [[nodiscard]] friend constexpr auto operator*(Matrix4x4f32 m,
                                                f32 const s) noexcept
    -> Matrix4x4f32 {
    return m *= s;
  }

  [[nodiscard]] friend constexpr auto operator*(f32 const f,
                                                Matrix4x4f32 const& m) noexcept
    -> Matrix4x4f32 {
    return m * f;
  }

  [[nodiscard]] friend constexpr auto operator/(Matrix4x4f32 l,
                                                Matrix4x4f32 const& r) noexcept
    -> Matrix4x4f32 {
    return l /= r;
  }

  [[nodiscard]] friend constexpr auto operator/(Matrix4x4f32 m,
                                                f32 const f) noexcept
    -> Matrix4x4f32 {
    return m /= f;
  }

  // determinant of the upper left 3x3 submatrix
  [[nodiscard]] constexpr auto det_3x3() const noexcept -> f32 {
    // clang-format off
    return m11 * (m22 * m33 - m23 * m32) -
           m12 * (m21 * m33 - m23 * m31) +
           m13 * (m21 * m32 - m22 * m31);
    // clang-format on
  }
};

} // namespace basalt
