#pragma once

#include "vector_p.h"

#include <basalt/api/base/types.h>

#include <cstddef>
#include <type_traits>
#include <utility>

namespace basalt {

class Vector3f32 : public detail::Vector<Vector3f32, f32, 3> {
public:
  static constexpr auto right() -> Vector3f32 {
    return Vector3f32{1.0f, 0.0f, 0.0f};
  }

  static constexpr auto left() -> Vector3f32 {
    return Vector3f32{-1.0f, 0.0f, 0.0f};
  }

  static constexpr auto up() -> Vector3f32 {
    return Vector3f32{0.0f, 1.0f, 0.0f};
  }

  static constexpr auto down() -> Vector3f32 {
    return Vector3f32{0.0f, -1.0f, 0.0f};
  }

  static constexpr auto forward() -> Vector3f32 {
    return Vector3f32{0.0f, 0.0f, 1.0f};
  }

  static constexpr auto back() -> Vector3f32 {
    return Vector3f32{0.0f, 0.0f, -1.0f};
  }

  constexpr Vector3f32() = default;

  constexpr explicit Vector3f32(f32 const v) : Vector{v, v, v} {
  }

  constexpr Vector3f32(f32 const x, f32 const y, f32 const z)
    : Vector{x, y, z} {
  }

  static constexpr auto cross(Vector3f32 const& l, Vector3f32 const& r)
    -> Vector3f32 {
    return l.cross(r);
  }

  constexpr auto cross(Vector3f32 const& r) const -> Vector3f32 {
    return Vector3f32{
      y() * r.z() - z() * r.y(),
      z() * r.x() - x() * r.z(),
      x() * r.y() - y() * r.x(),
    };
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
};

} // namespace basalt

template <>
struct std::tuple_size<basalt::Vector3f32>
  : std::integral_constant<std::size_t, 3> {};

template <std::size_t Idx>
struct std::tuple_element<Idx, basalt::Vector3f32> {
  static_assert(Idx < 3, "vector index out of bounds");
  using type = basalt::f32;
};
