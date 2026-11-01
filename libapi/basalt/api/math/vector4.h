#pragma once

#include "vector_p.h"

#include <basalt/api/base/types.h>

#include <cstddef>
#include <type_traits>
#include <utility>

namespace basalt {

class Vector4f32 : public detail::Vector<Vector4f32, f32, 4> {
public:
  constexpr Vector4f32() = default;

  constexpr explicit Vector4f32(f32 const v) : Vector{v, v, v, v} {
  }

  constexpr Vector4f32(f32 const x, f32 const y, f32 const z, f32 const w)
    : Vector{x, y, z, w} {
  }

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

template <>
struct std::tuple_size<basalt::Vector4f32>
  : std::integral_constant<std::size_t, 4> {};

template <std::size_t Idx>
struct std::tuple_element<Idx, basalt::Vector4f32> {
  static_assert(Idx < 4, "vector index out of bounds");
  using type = basalt::f32;
};
