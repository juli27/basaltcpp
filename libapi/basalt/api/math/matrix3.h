#pragma once

#include "matrix_p.h"

#include "matrix2.h"
#include "types.h"
#include "vector3.h"

#include <basalt/api/base/types.h>

namespace basalt {

class Matrix3x3f32 : public detail::Matrix<Matrix3x3f32, f32, 3, 3> {
public:
  static constexpr auto translation(f32 const x, f32 const y) -> Matrix3x3f32 {
    // clang-format off
    return Matrix3x3f32{1.0f, 0.0f, 0.0f,
                        0.0f, 1.0f, 0.0f,
                           x,    y, 1.0f};
    // clang-format on
  }

  static auto rotation(Angle x, Angle y, Angle z) -> Matrix3x3f32;
  static auto rotation_x(Angle) -> Matrix3x3f32;
  static auto rotation_y(Angle) -> Matrix3x3f32;
  static auto rotation_z(Angle) -> Matrix3x3f32;

  // axis must be a unit vector
  static auto rotation_axis(Vector3f32 const& axis, Angle) -> Matrix3x3f32;

  static constexpr auto shear_xy(f32 const xFactor, f32 const yFactor)
    -> Matrix3x3f32 {
    // clang-format off
    return Matrix3x3f32{   1.0f,    0.0f, 0.0f,
                           0.0f,    1.0f, 0.0f,
                        xFactor, yFactor, 1.0f};
    // clang-format on
  }

  static constexpr auto shear_xz(f32 const xFactor, f32 const zFactor)
    -> Matrix3x3f32 {
    // clang-format off
    return Matrix3x3f32{   1.0f, 0.0f,    0.0f,
                        xFactor, 1.0f, zFactor,
                           0.0f, 0.0f,    1.0f};
    // clang-format on
  }

  static constexpr auto shear_yz(f32 const yFactor, f32 const zFactor)
    -> Matrix3x3f32 {
    // clang-format off
    return Matrix3x3f32{1.0f, yFactor, zFactor,
                        0.0f,    1.0f,    0.0f,
                        0.0f,    0.0f,    1.0f};
    // clang-format on
  }

  // normal must be a unit vector
  static constexpr auto orthographic_projection(Vector3f32 const& normal)
    -> Matrix3x3f32 {
    // clang-format off
    return Matrix3x3f32{
      1.0f - normal.x() * normal.x(),
      -normal.x() * normal.y(),
      -normal.x() * normal.z(),

      -normal.x() * normal.y(),      
      1.0f - normal.y() * normal.y(),
      -normal.y() * normal.z(),

      -normal.x() * normal.z(),
      -normal.y() * normal.z(),
      1.0f - normal.z() * normal.z()};
    // clang-format on
  }

  static constexpr auto scale(f32 const factor) -> Matrix3x3f32 {
    return scale(factor, factor, factor);
  }

  static constexpr auto scale(f32 const x, f32 const y, f32 const z)
    -> Matrix3x3f32 {
    // clang-format off
    return Matrix3x3f32{   x, 0.0f, 0.0f,
                        0.0f,    y, 0.0f,
                        0.0f, 0.0f,    z};
    // clang-format on
  }

  // direction must be a unit vector
  static constexpr auto scale_direction(Vector3f32 const& direction,
                                        f32 const factor) -> Matrix3x3f32 {
    return Matrix3x3f32{1.0f + (factor - 1.0f) * direction.x() * direction.x(),
                        (factor - 1.0f) * direction.x() * direction.y(),
                        (factor - 1.0f) * direction.x() * direction.z(),

                        (factor - 1.0f) * direction.x() * direction.y(),
                        1.0f + (factor - 1.0f) * direction.y() * direction.y(),
                        (factor - 1.0f) * direction.y() * direction.z(),

                        (factor - 1.0f) * direction.x() * direction.z(),
                        (factor - 1.0f) * direction.y() * direction.z(),
                        1.0f + (factor - 1.0f) * direction.z() * direction.z()};
  }

  // m must be invertible
  static constexpr auto inverse(Matrix3x3f32 const& m) -> Matrix3x3f32 {
    // clang-format off
    auto result = Matrix3x3f32{ m.m22() * m.m33() - m.m23() * m.m32(),
                               -m.m12() * m.m33() + m.m13() * m.m32(),
                                m.m12() * m.m23() - m.m13() * m.m22(),

                               -m.m21() * m.m33() + m.m23() * m.m31(),
                                m.m11() * m.m33() - m.m13() * m.m31(),
                               -m.m11() * m.m23() + m.m13() * m.m21(),

                                m.m21() * m.m32() - m.m22() * m.m31(),
                               -m.m11() * m.m32() + m.m12() * m.m31(),
                                m.m11() * m.m22() - m.m12() * m.m21()};
    // clang-format on
    result /= m.det();

    return result;
  }

  static constexpr auto identity() -> Matrix3x3f32 {
    // clang-format off
    return Matrix3x3f32{1.0f, 0.0f, 0.0f,
                        0.0f, 1.0f, 0.0f,
                        0.0f, 0.0f, 1.0f};
    // clang-format on
  }

  constexpr Matrix3x3f32() = default;

  // clang-format off
  constexpr Matrix3x3f32(
     f32 const m11, f32 const m12, f32 const m13,
     f32 const m21, f32 const m22, f32 const m23,
     f32 const m31, f32 const m32, f32 const m33)
    : Matrix{m11, m12, m13,
             m21, m22, m23,
             m31, m32, m33} {
  }
  
  // clang-format off
  explicit constexpr Matrix3x3f32(Matrix2x2f32 const& o)
    : Matrix3x3f32{o.m11(), o.m12(), 0.0f,
                   o.m21(), o.m22(), 0.0f,
                      0.0f,    0.0f, 1.0f} {
  }

  // clang-format on

  // clang-format on

  constexpr auto det() const -> f32 {
    return m11() * (m22() * m33() - m23() * m32()) +
           m12() * (m23() * m31() - m21() * m33()) +
           m13() * (m21() * m32() - m22() * m31());
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
};

} // namespace basalt
