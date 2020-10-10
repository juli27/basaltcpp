#pragma once

#include "vector3.h"

#include "api/base/types.h"

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
    : m11(m11), m12(m12), m13(m13), m14(m14)
    , m21(m21), m22(m22), m23(m23), m24(m24)
    , m31(m31), m32(m32), m33(m33), m34(m34)
    , m41(m41), m42(m42), m43(m43), m44(m44) {
  }

  // clang-format on

  auto operator+=(const Mat4&) noexcept -> Mat4&;
  auto operator-=(const Mat4&) noexcept -> Mat4&;
  auto operator*=(const Mat4&) noexcept -> Mat4&;
  auto operator*=(f32) noexcept -> Mat4&;
  auto operator/=(const Mat4&) noexcept -> Mat4&;
  auto operator/=(f32) noexcept -> Mat4&;

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

  [[nodiscard]] static auto invert(const Mat4&) noexcept -> Mat4;

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
};

using Mat4f32 = Mat4;

constexpr auto operator-(const Mat4& m) noexcept -> Mat4 {
  // clang-format off
  return Mat4 {-m.m11, -m.m12, -m.m13, -m.m14,
               -m.m21, -m.m22, -m.m23, -m.m24,
               -m.m31, -m.m32, -m.m33, -m.m34,
               -m.m41, -m.m42, -m.m43, -m.m44};
  // clang-format on
}

inline auto operator+(Mat4 l, const Mat4& r) noexcept -> Mat4 {
  l += r;
  return l;
}

inline auto operator-(Mat4 l, const Mat4& r) noexcept -> Mat4 {
  l -= r;
  return l;
}

inline auto operator*(Mat4 l, const Mat4& r) noexcept -> Mat4 {
  l *= r;
  return l;
}

inline auto operator*(Mat4 m, const f32 s) noexcept -> Mat4 {
  m *= s;
  return m;
}

inline Mat4 operator*(const f32 f, const Mat4& m) noexcept {
  return m * f;
}

auto operator/(const Mat4& a, const Mat4& b) noexcept -> Mat4;

inline auto operator/(Mat4 m, const f32 f) noexcept -> Mat4 {
  m /= f;
  return m;
}

constexpr auto operator==(const Mat4& l, const Mat4& r) noexcept -> bool {
  return l.m11 == r.m11 && l.m12 == r.m12 && l.m13 == r.m13 && l.m14 == r.m14 &&
         l.m21 == r.m21 && l.m22 == r.m22 && l.m23 == r.m23 && l.m24 == r.m24 &&
         l.m31 == r.m31 && l.m32 == r.m32 && l.m33 == r.m33 && l.m34 == r.m34 &&
         l.m41 == r.m41 && l.m42 == r.m42 && l.m43 == r.m43 && l.m44 == r.m44;
}

constexpr auto operator!=(const Mat4& l, const Mat4& r) noexcept -> bool {
  return !(l == r);
}

} // namespace basalt
