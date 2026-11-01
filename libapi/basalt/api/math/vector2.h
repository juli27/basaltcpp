#pragma once

#include "vector_p.h"

#include <basalt/api/base/types.h>

#include <cstddef>
#include <type_traits>
#include <utility>

namespace basalt {

class Vector2f32 : public detail::Vector<Vector2f32, f32, 2> {
public:
  constexpr Vector2f32() = default;

  constexpr explicit Vector2f32(f32 const v) : Vector{v, v} {
  }

  constexpr Vector2f32(f32 const x, f32 const y) : Vector{x, y} {
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
};

} // namespace basalt

template <>
struct std::tuple_size<basalt::Vector2f32>
  : std::integral_constant<std::size_t, 2> {};

template <std::size_t Idx>
struct std::tuple_element<Idx, basalt::Vector2f32> {
  static_assert(Idx < 2, "vector index out of bounds");
  using type = basalt::f32;
};
