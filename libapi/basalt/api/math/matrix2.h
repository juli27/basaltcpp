#pragma once

#include "matrix_p.h"

#include "types.h"
#include "vector2.h"

#include <basalt/api/base/types.h>

namespace basalt {

class Matrix2x2f32 : public detail::Matrix<Matrix2x2f32, f32, 2, 2> {
public:
  static auto rotation(Angle) -> Matrix2x2f32;

  static constexpr auto shear_x(f32 const factor) -> Matrix2x2f32 {
    // clang-format off
    return Matrix2x2f32{  1.0f, 0.0f,
                        factor, 1.0f};
    // clang-format on
  }

  static constexpr auto shear_y(f32 const factor) -> Matrix2x2f32 {
    // clang-format off
    return Matrix2x2f32{1.0f, factor,
                        0.0f,   1.0f};
    // clang-format on
  }

  // normal must be a unit vector
  static constexpr auto orthographic_projection(Vector2f32 const& normal)
    -> Matrix2x2f32 {
    return Matrix2x2f32{1.0f - normal.x() * normal.x(),
                        -normal.x() * normal.y(), -normal.x() * normal.y(),
                        1.0f - normal.y() * normal.y()};
  }

  static constexpr auto scale(f32 const factor) -> Matrix2x2f32 {
    return scale(factor, factor);
  }

  static constexpr auto scale(f32 const x, f32 const y) -> Matrix2x2f32 {
    // clang-format off
    return Matrix2x2f32{   x, 0.0f,
                        0.0f,    y};
    // clang-format on
  }

  // direction must be a unit vector
  static constexpr auto scale_direction(Vector2f32 const& direction,
                                        f32 const factor) -> Matrix2x2f32 {
    return Matrix2x2f32{1.0f + (factor - 1.0f) * direction.x() * direction.x(),
                        (factor - 1.0f) * direction.x() * direction.y(),

                        (factor - 1.0f) * direction.x() * direction.y(),
                        1.0f + (factor - 1.0f) * direction.y() * direction.y()};
  }

  // m must be invertible
  static constexpr auto inverse(Matrix2x2f32 const& m) -> Matrix2x2f32 {
    // clang-format off
    auto result = Matrix2x2f32{ m.m22(), -m.m12(),
                               -m.m21(), m.m11()};
    // clang-format on
    result /= m.det();

    return result;
  }

  static constexpr auto identity() -> Matrix2x2f32 {
    // clang-format off
    return Matrix2x2f32{1.0f, 0.0f,
                        0.0f, 1.0f};
    // clang-format on
  }

  constexpr Matrix2x2f32() = default;

  // clang-format off
  constexpr Matrix2x2f32(
     f32 const m11, f32 const m12,
     f32 const m21, f32 const m22)
    : Matrix{m11, m12,
             m21, m22} {
  }

  // clang-format on

  constexpr auto det() const -> f32 {
    return m11() * m22() - m12() * m21();
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
};

} // namespace basalt
