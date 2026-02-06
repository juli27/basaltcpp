#pragma once

#include "matrix_p.h"

#include "matrix3.h"
#include "types.h"
#include "vector3.h"

#include <basalt/api/base/types.h>

namespace basalt {

class Matrix4x4f32 : public detail::Matrix<Matrix4x4f32, f32, 4, 4> {
public:
  static constexpr auto translation(f32 const x, f32 const y, f32 const z)
    -> Matrix4x4f32 {
    // clang-format off
    return Matrix4x4f32{1.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 1.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 1.0f, 0.0f,
                           x,    y,    z, 1.0f};
    // clang-format on
  }

  static constexpr auto translation(Vector3f32 const& t) -> Matrix4x4f32 {
    return translation(t.x(), t.y(), t.z());
  }

  static auto look_at_lh(Vector3f32 const& position, Vector3f32 const& lookAt,
                         Vector3f32 const& up) -> Matrix4x4f32;

  static auto perspective_projection(Angle fov, f32 aspectRatio, f32 nearPlaneZ,
                                     f32 farPlaneZ) -> Matrix4x4f32;

  static constexpr auto identity() -> Matrix4x4f32 {
    // clang-format off
    return Matrix4x4f32{1.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 1.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 1.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 1.0f};
    // clang-format on
  }

  constexpr Matrix4x4f32() = default;

  // clang-format off
  constexpr Matrix4x4f32(
     f32 const m11, f32 const m12, f32 const m13, f32 const m14,
     f32 const m21, f32 const m22, f32 const m23, f32 const m24,
     f32 const m31, f32 const m32, f32 const m33, f32 const m34,
     f32 const m41, f32 const m42, f32 const m43, f32 const m44)
    : Matrix{m11, m12, m13, m14,
             m21, m22, m23, m24,
             m31, m32, m33, m34,
             m41, m42, m43, m44} {
  }

  // clang-format on

  // clang-format off
  explicit constexpr Matrix4x4f32(Matrix3x3f32 const& o)
    : Matrix4x4f32{o.m11(), o.m12(), o.m13(), 0.0f,
                   o.m21(), o.m22(), o.m23(), 0.0f,
                   o.m31(), o.m32(), o.m33(), 0.0f,
                      0.0f,    0.0f,    0.0f, 1.0f} {
  }

  // clang-format on

  constexpr auto det() const -> f32 {
    auto const minor11 = m22() * (m33() * m44() - m34() * m43()) +
                         m23() * (m34() * m42() - m32() * m44()) +
                         m24() * (m32() * m43() - m33() * m42());
    auto const minor12 = m21() * (m33() * m44() - m34() * m43()) +
                         m23() * (m34() * m41() - m31() * m44()) +
                         m24() * (m31() * m43() - m33() * m41());
    auto const minor13 = m21() * (m32() * m44() - m34() * m42()) +
                         m22() * (m34() * m41() - m31() * m44()) +
                         m24() * (m31() * m42() - m32() * m41());
    auto const minor14 = m21() * (m32() * m43() - m33() * m42()) +
                         m22() * (m33() * m41() - m31() * m43()) +
                         m23() * (m31() * m42() - m32() * m41());

    return m11() * minor11 - m12() * minor12 + m13() * minor13 -
           m14() * minor14;
  }

  constexpr auto m11() const -> f32 const& {
    return get<0, 0>();
  }

  constexpr auto m11() -> f32& {
    return get<0, 0>();
  }

  constexpr auto m12() const -> f32 const& {
    return get<0, 1>();
  }

  constexpr auto m12() -> f32& {
    return get<0, 1>();
  }

  constexpr auto m13() const -> f32 const& {
    return get<0, 2>();
  }

  constexpr auto m13() -> f32& {
    return get<0, 2>();
  }

  constexpr auto m14() const -> f32 const& {
    return get<0, 3>();
  }

  constexpr auto m14() -> f32& {
    return get<0, 3>();
  }

  constexpr auto m21() const -> f32 const& {
    return get<1, 0>();
  }

  constexpr auto m21() -> f32& {
    return get<1, 0>();
  }

  constexpr auto m22() const -> f32 const& {
    return get<1, 1>();
  }

  constexpr auto m22() -> f32& {
    return get<1, 1>();
  }

  constexpr auto m23() const -> f32 const& {
    return get<1, 2>();
  }

  constexpr auto m23() -> f32& {
    return get<1, 2>();
  }

  constexpr auto m24() const -> f32 const& {
    return get<1, 3>();
  }

  constexpr auto m24() -> f32& {
    return get<1, 3>();
  }

  constexpr auto m31() const -> f32 const& {
    return get<2, 0>();
  }

  constexpr auto m31() -> f32& {
    return get<2, 0>();
  }

  constexpr auto m32() const -> f32 const& {
    return get<2, 1>();
  }

  constexpr auto m32() -> f32& {
    return get<2, 1>();
  }

  constexpr auto m33() const -> f32 const& {
    return get<2, 2>();
  }

  constexpr auto m33() -> f32& {
    return get<2, 2>();
  }

  constexpr auto m34() const -> f32 const& {
    return get<2, 3>();
  }

  constexpr auto m34() -> f32& {
    return get<2, 3>();
  }

  constexpr auto m41() const -> f32 const& {
    return get<3, 0>();
  }

  constexpr auto m41() -> f32& {
    return get<3, 0>();
  }

  constexpr auto m42() const -> f32 const& {
    return get<3, 1>();
  }

  constexpr auto m42() -> f32& {
    return get<3, 1>();
  }

  constexpr auto m43() const -> f32 const& {
    return get<3, 2>();
  }

  constexpr auto m43() -> f32& {
    return get<3, 2>();
  }

  constexpr auto m44() const -> f32 const& {
    return get<3, 3>();
  }

  constexpr auto m44() -> f32& {
    return get<3, 3>();
  }
};

} // namespace basalt
