#pragma once

#include "angle.h"

#include <basalt/api/base/types.h>

#include <array>

namespace basalt::detail {

template <typename Derived, typename T, uSize Dimension>
class Vector {
public:
  static auto constexpr DIMENSION = Dimension;

  static constexpr auto zero() -> Derived {
    return Derived{};
  }

  static auto normalized(Derived nonZeroVec) -> Derived;
  auto normalize() -> Derived&;

  auto angle_to(Derived const&) const -> Angle;
  auto distance_to(Derived const&) const -> T;
  auto length() const -> T;

  constexpr auto length_squared() const -> T {
    return dot(self());
  }

  constexpr auto dot(Derived const& r) const -> T {
    auto dotProduct = T{};
    for (auto i = uSize{0}; i < DIMENSION; ++i) {
      dotProduct += mComponents[i] * r.mComponents[i];
    }

    return dotProduct;
  }

  constexpr auto mul(T const& scalar) -> Derived& {
    for (auto& component : mComponents) {
      component *= scalar;
    }

    return self();
  }

  constexpr auto operator*=(T const& scalar) -> Derived& {
    return mul(scalar);
  }

  friend constexpr auto operator*(Derived v, T const& scalar) -> Derived {
    v *= scalar;
    return v;
  }

  friend constexpr auto operator*(T const& scalar, Derived const& v)
    -> Derived {
    return v * scalar;
  }

  constexpr auto div(T const& scalar) -> Derived& {
    for (auto& component : mComponents) {
      component /= scalar;
    }

    return self();
  }

  constexpr auto operator/=(T const& scalar) -> Derived& {
    return div(scalar);
  }

  friend constexpr auto operator/(Derived v, T const& scalar) -> Derived {
    v /= scalar;
    return v;
  }

  constexpr auto add(Derived const& r) -> Derived& {
    for (auto i = uSize{0}; i < DIMENSION; ++i) {
      mComponents[i] += r.mComponents[i];
    }

    return self();
  }

  constexpr auto operator+=(Derived const& r) -> Derived& {
    return add(r);
  }

  friend constexpr auto operator+(Derived l, Derived const& r) -> Derived {
    l += r;
    return l;
  }

  constexpr auto sub(Derived const& r) -> Derived& {
    for (auto i = uSize{0}; i < DIMENSION; ++i) {
      mComponents[i] -= r.mComponents[i];
    }

    return self();
  }

  constexpr auto operator-=(Derived const& r) -> Derived& {
    return sub(r);
  }

  friend constexpr auto operator-(Derived l, Derived const& r) -> Derived {
    l -= r;
    return l;
  }

  constexpr auto negate() -> Derived& {
    for (auto& component : mComponents) {
      component = -component;
    }

    return self();
  }

  friend constexpr auto operator-(Derived v) -> Derived {
    v.negate();
    return v;
  }

  friend constexpr auto operator==(Derived const& lhs, Derived const& rhs)
    -> bool {
    return lhs.mComponents == rhs.mComponents;
  }

  friend constexpr auto operator!=(Derived const& lhs, Derived const& rhs)
    -> bool {
    return !(lhs == rhs);
  }

  template <uSize Idx>
  constexpr auto get() const -> T const& {
    static_assert(Idx < DIMENSION, "Vector index out of bounds");
    return mComponents[Idx];
  }

  template <uSize Idx>
  constexpr auto get() -> T& {
    static_assert(Idx < DIMENSION, "Vector index out of bounds");
    return mComponents[Idx];
  }

  constexpr auto operator[](uSize const idx) const -> T const& {
    return mComponents[idx];
  }

  constexpr auto operator[](uSize const idx) -> T& {
    return mComponents[idx];
  }

  constexpr auto data() const -> T const* {
    return mComponents.data();
  }

  constexpr auto data() -> T* {
    return mComponents.data();
  }

protected:
  constexpr auto self() const -> Derived const& {
    return static_cast<Derived const&>(*this);
  }

  constexpr auto self() -> Derived& {
    return static_cast<Derived&>(*this);
  }

private:
  friend Derived;

  constexpr Vector() = default;

  template <typename... Components>
  constexpr explicit Vector(Components... components)
    : mComponents{components...} {
    static_assert(sizeof...(Components) == DIMENSION);
  }

  std::array<T, DIMENSION> mComponents{};
};

} // namespace basalt::detail
