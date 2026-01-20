#pragma once

#include "vector.h"

#include <basalt/api/base/types.h>

namespace basalt {

class Vector4f32 : public detail::Vector<Vector4f32, f32, 4> {
public:
  constexpr Vector4f32() = default;

  constexpr explicit Vector4f32(f32 const v) : Vector{v, v, v, v} {
  }

  constexpr Vector4f32(f32 const x, f32 const y, f32 const z, f32 const w)
    : Vector{x, y, z, w} {
  }

  static auto normalized(f32 x, f32 y, f32 z, f32 w) -> Vector4f32;
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

  constexpr auto z() const -> f32 const& {
    return get<2>();
  }

  constexpr auto z() -> f32& {
    return get<2>();
  }

  constexpr auto w() const -> f32 const& {
    return get<3>();
  }

  constexpr auto w() -> f32& {
    return get<3>();
  }
};

} // namespace basalt
