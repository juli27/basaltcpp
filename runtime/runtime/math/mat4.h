#pragma once

#include "vec3.h"

#include <runtime/shared/types.h>

namespace basalt {

struct Mat4 final {
  f32 m11 {}, m12 {}, m13 {}, m14 {};
  f32 m21 {}, m22 {}, m23 {}, m24 {};
  f32 m31 {}, m32 {}, m33 {}, m34 {};
  f32 m41 {}, m42 {}, m43 {}, m44 {};

  constexpr Mat4() noexcept = default;

  constexpr Mat4(
    const f32 m11, const f32 m12, const f32 m13, const f32 m14
  , const f32 m21, const f32 m22, const f32 m23, const f32 m24
  , const f32 m31, const f32 m32, const f32 m33, const f32 m34
  , const f32 m41, const f32 m42, const f32 m43, const f32 m44
  ) noexcept
    : m11(m11), m12(m12), m13(m13), m14(m14)
    , m21(m21), m22(m22), m23(m23), m24(m24)
    , m31(m31), m32(m32), m33(m33), m34(m34)
    , m41(m41), m42(m42), m43(m43), m44(m44) {
  }

  constexpr Mat4(const Mat4&) noexcept = default;
  constexpr Mat4(Mat4&&) noexcept = default;

  ~Mat4() noexcept = default;

  auto operator=(const Mat4&) noexcept -> Mat4& = default;
  auto operator=(Mat4&&) noexcept -> Mat4& = default;

  auto operator+=(const Mat4&) noexcept -> Mat4&;
  auto operator-=(const Mat4&) noexcept -> Mat4&;
  auto operator*=(const Mat4&) noexcept -> Mat4&;
  auto operator*=(f32) noexcept -> Mat4&;
  auto operator/=(const Mat4&) noexcept -> Mat4&;
  auto operator/=(f32) noexcept -> Mat4&;

  // TODO: fix
  // Determinante der linken oberen 3x3-Teilmatrix berechnen
  [[nodiscard]]
  constexpr auto det() const noexcept -> f32 {
    return m11 * (m22 * m33 - m23 * m32)
      - m12 * (m21 * m33 - m23 * m31)
      + m13 * (m21 * m32 - m22 * m31);
  }

  [[nodiscard]]
  static constexpr auto identity() noexcept -> Mat4 {
    return Mat4 {
      1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f
    };
  }

  [[nodiscard]]
  static auto invert(const Mat4&) noexcept -> Mat4;

  [[nodiscard]]
  static constexpr auto translation(const Vec3f32& t) noexcept -> Mat4 {
    return Mat4 {
      1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f,
       t.x,  t.y,  t.z, 1.0f
    };
  }

  [[nodiscard]]
  static auto rotation_x(f32 radians) -> Mat4;

  [[nodiscard]]
  static auto rotation_y(f32 radians) -> Mat4;

  [[nodiscard]]
  static auto rotation_z(f32 radians) -> Mat4;

  [[nodiscard]]
  static auto rotation(const Vec3f32& radians) -> Mat4;

  [[nodiscard]]
  static constexpr auto scaling(const Vec3f32& s) noexcept -> Mat4 {
    return Mat4 {
       s.x, 0.0f, 0.0f, 0.0f,
      0.0f,  s.y, 0.0f, 0.0f,
      0.0f, 0.0f,  s.z, 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f
    };
  }

  [[nodiscard]]
  static auto perspective_projection(
    f32 fovRadians, f32 aspectRatio, f32 nearPlane, f32 farPlane) -> Mat4;
};

using Mat4f32 = Mat4;

constexpr auto operator-(const Mat4& m) noexcept -> Mat4 {
  return Mat4 {
    -m.m11, -m.m12, -m.m13, -m.m14,
    -m.m21, -m.m22, -m.m23, -m.m24,
    -m.m31, -m.m32, -m.m33, -m.m34,
    -m.m41, -m.m42, -m.m43, -m.m44
  };
}

constexpr auto operator+(const Mat4& l, const Mat4& r) noexcept -> Mat4 {
  return Mat4 {
    l.m11 + r.m11, l.m12 + r.m12, l.m13 + r.m13, l.m14 + r.m14,
    l.m21 + r.m21, l.m22 + r.m22, l.m23 + r.m23, l.m24 + r.m24,
    l.m31 + r.m31, l.m32 + r.m32, l.m33 + r.m33, l.m34 + r.m34,
    l.m41 + r.m41, l.m42 + r.m42, l.m43 + r.m43, l.m44 + r.m44
  };
}

constexpr auto operator-(const Mat4& l, const Mat4& r) noexcept -> Mat4 {
  return Mat4 {
    l.m11 - r.m11, l.m12 - r.m12, l.m13 - r.m13, l.m14 - r.m14,
    l.m21 - r.m21, l.m22 - r.m22, l.m23 - r.m23, l.m24 - r.m24,
    l.m31 - r.m31, l.m32 - r.m32, l.m33 - r.m33, l.m34 - r.m34,
    l.m41 - r.m41, l.m42 - r.m42, l.m43 - r.m43, l.m44 - r.m44
  };
}

constexpr auto operator*(const Mat4& l, const Mat4& r) noexcept -> Mat4 {
  return Mat4 {
    r.m11 * l.m11 + r.m21 * l.m12 + r.m31 * l.m13 + r.m41 * l.m14,
    r.m12 * l.m11 + r.m22 * l.m12 + r.m32 * l.m13 + r.m42 * l.m14,
    r.m13 * l.m11 + r.m23 * l.m12 + r.m33 * l.m13 + r.m43 * l.m14,
    r.m14 * l.m11 + r.m24 * l.m12 + r.m34 * l.m13 + r.m44 * l.m14,

    r.m11 * l.m21 + r.m21 * l.m22 + r.m31 * l.m23 + r.m41 * l.m24,
    r.m12 * l.m21 + r.m22 * l.m22 + r.m32 * l.m23 + r.m42 * l.m24,
    r.m13 * l.m21 + r.m23 * l.m22 + r.m33 * l.m23 + r.m43 * l.m24,
    r.m14 * l.m21 + r.m24 * l.m22 + r.m34 * l.m23 + r.m44 * l.m24,

    r.m11 * l.m31 + r.m21 * l.m32 + r.m31 * l.m33 + r.m41 * l.m34,
    r.m12 * l.m31 + r.m22 * l.m32 + r.m32 * l.m33 + r.m42 * l.m34,
    r.m13 * l.m31 + r.m23 * l.m32 + r.m33 * l.m33 + r.m43 * l.m34,
    r.m14 * l.m31 + r.m24 * l.m32 + r.m34 * l.m33 + r.m44 * l.m34,

    r.m11 * l.m41 + r.m21 * l.m42 + r.m31 * l.m43 + r.m41 * l.m44,
    r.m12 * l.m41 + r.m22 * l.m42 + r.m32 * l.m43 + r.m42 * l.m44,
    r.m13 * l.m41 + r.m23 * l.m42 + r.m33 * l.m43 + r.m43 * l.m44,
    r.m14 * l.m41 + r.m24 * l.m42 + r.m34 * l.m43 + r.m44 * l.m44
  };
}

constexpr auto operator*(const Mat4& m, const f32 s) noexcept -> Mat4 {
  return Mat4 {
    m.m11 * s, m.m12 * s, m.m13 * s, m.m14 * s,
    m.m21 * s, m.m22 * s, m.m23 * s, m.m24 * s,
    m.m31 * s, m.m32 * s, m.m33 * s, m.m34 * s,
    m.m41 * s, m.m42 * s, m.m43 * s, m.m44 * s
  };
}

constexpr Mat4 operator*(const f32 f, const Mat4& m) noexcept {
  return m * f;
}

auto operator/(const Mat4& a, const Mat4& b) noexcept -> Mat4;

constexpr auto operator/(const Mat4& m, const f32 f) noexcept -> Mat4 {
  return Mat4 {
    m.m11 / f, m.m12 / f, m.m13 / f, m.m14 / f,
    m.m21 / f, m.m22 / f, m.m23 / f, m.m24 / f,
    m.m31 / f, m.m32 / f, m.m33 / f, m.m34 / f,
    m.m41 / f, m.m42 / f, m.m43 / f, m.m44 / f
  };
}

constexpr auto operator==(const Mat4& l, const Mat4& r) noexcept -> bool {
  if (l.m11 != r.m11) return false;
  if (l.m12 != r.m12) return false;
  if (l.m13 != r.m13) return false;
  if (l.m14 != r.m14) return false;
  if (l.m21 != r.m21) return false;
  if (l.m22 != r.m22) return false;
  if (l.m23 != r.m23) return false;
  if (l.m24 != r.m24) return false;
  if (l.m31 != r.m31) return false;
  if (l.m32 != r.m32) return false;
  if (l.m33 != r.m33) return false;
  if (l.m34 != r.m34) return false;
  if (l.m41 != r.m41) return false;
  if (l.m42 != r.m42) return false;
  if (l.m43 != r.m43) return false;

  return l.m44 == r.m44;
}

constexpr auto operator!=(const Mat4& l, const Mat4& r) noexcept -> bool {
  if (l.m11 != r.m11) return true;
  if (l.m12 != r.m12) return true;
  if (l.m13 != r.m13) return true;
  if (l.m14 != r.m14) return true;
  if (l.m21 != r.m21) return true;
  if (l.m22 != r.m22) return true;
  if (l.m23 != r.m23) return true;
  if (l.m24 != r.m24) return true;
  if (l.m31 != r.m31) return true;
  if (l.m32 != r.m32) return true;
  if (l.m33 != r.m33) return true;
  if (l.m34 != r.m34) return true;
  if (l.m41 != r.m41) return true;
  if (l.m42 != r.m42) return true;
  if (l.m43 != r.m43) return true;

  return l.m44 != r.m44;
}

} // namespace basalt
