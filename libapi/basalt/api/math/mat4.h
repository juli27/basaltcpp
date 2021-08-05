#pragma once

#include <basalt/api/math/types.h>
#include <basalt/api/math/vector3.h>

#include <basalt/api/base/types.h>

namespace basalt {

struct Mat4 final {
  f32 m11 {}, m12 {}, m13 {}, m14 {};
  f32 m21 {}, m22 {}, m23 {}, m24 {};
  f32 m31 {}, m32 {}, m33 {}, m34 {};
  f32 m41 {}, m42 {}, m43 {}, m44 {};

  constexpr Mat4() noexcept = default;

  // clang-format off
  constexpr Mat4(const f32 m11, const f32 m12, const f32 m13, const f32 m14,
                 const f32 m21, const f32 m22, const f32 m23, const f32 m24,
                 const f32 m31, const f32 m32, const f32 m33, const f32 m34,
                 const f32 m41, const f32 m42, const f32 m43, const f32 m44)
  noexcept
    : m11 {m11}, m12 {m12}, m13 {m13}, m14 {m14}
    , m21 {m21}, m22 {m22}, m23 {m23}, m24 {m24}
    , m31 {m31}, m32 {m32}, m33 {m33}, m34 {m34}
    , m41 {m41}, m42 {m42}, m43 {m43}, m44 {m44} {
  }
  // clang-format on

  constexpr auto operator+=(const Mat4& rhs) noexcept -> Mat4& {
    // clang-format off
    m11 += rhs.m11; m12 += rhs.m12; m13 += rhs.m13; m14 += rhs.m14;
    m21 += rhs.m21; m22 += rhs.m22; m23 += rhs.m23; m24 += rhs.m24;
    m31 += rhs.m31; m32 += rhs.m32; m33 += rhs.m33; m34 += rhs.m34;
    m41 += rhs.m41; m42 += rhs.m42; m43 += rhs.m43; m44 += rhs.m44;
    // clang-format on

    return *this;
  }

  constexpr auto operator-=(const Mat4& rhs) noexcept -> Mat4& {
    // clang-format off
    m11 -= rhs.m11; m12 -= rhs.m12; m13 -= rhs.m13; m14 -= rhs.m14;
    m21 -= rhs.m21; m22 -= rhs.m22; m23 -= rhs.m23; m24 -= rhs.m24;
    m31 -= rhs.m31; m32 -= rhs.m32; m33 -= rhs.m33; m34 -= rhs.m34;
    m41 -= rhs.m41; m42 -= rhs.m42; m43 -= rhs.m43; m44 -= rhs.m44;
    // clang-format on

    return *this;
  }

  constexpr auto operator*=(const Mat4& rhs) noexcept -> Mat4& {
    *this =
      Mat4 {rhs.m11 * m11 + rhs.m21 * m12 + rhs.m31 * m13 + rhs.m41 * m14,
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
            rhs.m14 * m41 + rhs.m24 * m42 + rhs.m34 * m43 + rhs.m44 * m44};

    return *this;
  }

  constexpr auto operator*=(const f32 rhs) noexcept -> Mat4& {
    // clang-format off
    m11 *= rhs; m12 *= rhs; m13 *= rhs; m14 *= rhs;
    m21 *= rhs; m22 *= rhs; m23 *= rhs; m24 *= rhs;
    m31 *= rhs; m32 *= rhs; m33 *= rhs; m34 *= rhs;
    m41 *= rhs; m42 *= rhs; m43 *= rhs; m44 *= rhs;
    // clang-format on

    return *this;
  }

  constexpr auto operator/=(const Mat4& rhs) noexcept -> Mat4& {
    *this *= invert(rhs);
    return *this;
  }

  constexpr auto operator/=(const f32 rhs) noexcept -> Mat4& {
    // clang-format off
    m11 /= rhs; m12 /= rhs; m13 /= rhs; m14 /= rhs;
    m21 /= rhs; m22 /= rhs; m23 /= rhs; m24 /= rhs;
    m31 /= rhs; m32 /= rhs; m33 /= rhs; m34 /= rhs;
    m41 /= rhs; m42 /= rhs; m43 /= rhs; m44 /= rhs;
    // clang-format on

    return *this;
  }

  [[nodiscard]] constexpr auto operator==(const Mat4& r) const noexcept
    -> bool {
    return m11 == r.m11 && m12 == r.m12 && m13 == r.m13 && m14 == r.m14 &&
           m21 == r.m21 && m22 == r.m22 && m23 == r.m23 && m24 == r.m24 &&
           m31 == r.m31 && m32 == r.m32 && m33 == r.m33 && m34 == r.m34 &&
           m41 == r.m41 && m42 == r.m42 && m43 == r.m43 && m44 == r.m44;
  }

  [[nodiscard]] constexpr auto operator!=(const Mat4& r) const noexcept
    -> bool {
    return !(*this == r);
  }

  [[nodiscard]] constexpr auto operator-() noexcept -> Mat4 {
    // clang-format off
    return Mat4 {-m11, -m12, -m13, -m14,
                 -m21, -m22, -m23, -m24,
                 -m31, -m32, -m33, -m34,
                 -m41, -m42, -m43, -m44};
    // clang-format on
  }

  // TODO: fix
  // Determinante der linken oberen 3x3-Teilmatrix berechnen
  [[nodiscard]] constexpr auto det() const noexcept -> f32 {
    return m11 * (m22 * m33 - m23 * m32) - m12 * (m21 * m33 - m23 * m31) +
           m13 * (m21 * m32 - m22 * m31);
  }

  [[nodiscard]] static constexpr auto identity() noexcept -> Mat4 {
    // clang-format off
    return Mat4 {1.0f, 0.0f, 0.0f, 0.0f,
                 0.0f, 1.0f, 0.0f, 0.0f,
                 0.0f, 0.0f, 1.0f, 0.0f,
                 0.0f, 0.0f, 0.0f, 1.0f};
    // clang-format on
  }

  [[nodiscard]] static constexpr auto invert(const Mat4& m) noexcept -> Mat4 {
    auto invDet {m.det()};
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

    result.m41 =
      -(m.m41 * result.m11 + m.m42 * result.m21 + m.m43 * result.m31);
    result.m42 =
      -(m.m41 * result.m12 + m.m42 * result.m22 + m.m43 * result.m32);
    result.m43 =
      -(m.m41 * result.m13 + m.m42 * result.m23 + m.m43 * result.m33);
    result.m44 = 1.0f;

    return result;
  }

  [[nodiscard]] static constexpr auto translation(const Vector3f32& t) noexcept
    -> Mat4 {
    // clang-format off
    return Mat4 { 1.0f,  0.0f,  0.0f, 0.0f,
                  0.0f,  1.0f,  0.0f, 0.0f,
                  0.0f,  0.0f,  1.0f, 0.0f,
                 t.x(), t.y(), t.z(), 1.0f};
    // clang-format on
  }

  [[nodiscard]] static auto rotation_x(f32 radians) noexcept -> Mat4;

  [[nodiscard]] static auto rotation_y(f32 radians) noexcept -> Mat4;

  [[nodiscard]] static auto rotation_z(f32 radians) noexcept -> Mat4;

  [[nodiscard]] static auto rotation(const Vector3f32& radians) noexcept
    -> Mat4;

  [[nodiscard]] static constexpr auto scaling(const Vector3f32& s) noexcept
    -> Mat4 {
    // clang-format off
    return Mat4 {s.x(),  0.0f,  0.0f, 0.0f,
                  0.0f, s.y(),  0.0f, 0.0f,
                  0.0f,  0.0f, s.z(), 0.0f,
                  0.0f,  0.0f,  0.0f, 1.0f};
    // clang-format on
  }

  [[nodiscard]] static auto
  perspective_projection(f32 fovRadians, f32 aspectRatio, f32 nearPlane,
                         f32 farPlane) noexcept -> Mat4;

  [[nodiscard]] friend constexpr auto operator+(Mat4 l, const Mat4& r) noexcept
    -> Mat4 {
    l += r;
    return l;
  }

  [[nodiscard]] friend constexpr auto operator-(Mat4 l, const Mat4& r) noexcept
    -> Mat4 {
    l -= r;
    return l;
  }

  [[nodiscard]] friend constexpr auto operator*(Mat4 l, const Mat4& r) noexcept
    -> Mat4 {
    l *= r;
    return l;
  }

  [[nodiscard]] friend constexpr auto operator*(Mat4 m, const f32 s) noexcept
    -> Mat4 {
    m *= s;
    return m;
  }

  [[nodiscard]] friend constexpr auto operator*(const f32 f,
                                                const Mat4& m) noexcept
    -> Mat4 {
    return m * f;
  }

  [[nodiscard]] friend constexpr auto operator/(const Mat4& a,
                                                const Mat4& b) noexcept
    -> Mat4 {
    return a * Mat4::invert(b);
  }

  [[nodiscard]] friend constexpr auto operator/(Mat4 m, const f32 f) noexcept
    -> Mat4 {
    m /= f;
    return m;
  }
};

} // namespace basalt
