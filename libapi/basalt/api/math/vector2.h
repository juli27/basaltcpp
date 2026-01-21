#pragma once

#include "vector_p.h"

#include <basalt/api/base/types.h>

namespace basalt {

class Vector2f32 : public detail::Vector<Vector2f32, f32, 2> {
public:
  constexpr Vector2f32() = default;

  constexpr explicit Vector2f32(f32 const v) : Vector{v, v} {
  }

  constexpr Vector2f32(f32 const x, f32 const y) : Vector{x, y} {
  }

  static auto normalized(f32 x, f32 y) -> Vector2f32;
  using Vector::normalized;

  constexpr auto x() const -> f32 const& {
    return get<0>();
  }

  constexpr auto x() -> f32& {
    return get<0>();
  }

  constexpr auto y() const -> f32 const& {
    return get<1>();
  }

  constexpr auto y() -> f32& {
    return get<1>();
  }
};

} // namespace basalt
