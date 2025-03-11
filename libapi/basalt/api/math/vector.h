#pragma once

#include "types.h"

#include "basalt/api/base/types.h"

#include <array>

namespace basalt::detail {

template <typename Derived, typename T, uSize Size>
struct Vector {
  // this must be an aggregate type

  [[nodiscard]]
  static constexpr auto zero() noexcept -> Derived {
    return Derived{};
  }

  std::array<T, Size> components{};

  [[nodiscard]] constexpr auto operator==(Derived const& o) const noexcept
    -> bool {
    for (auto i = uSize{0}; i < components.size(); ++i) {
      if (components[i] != o.components[i]) {
        return false;
      }
    }

    return true;
  }

  [[nodiscard]] constexpr auto operator!=(Derived const& o) const noexcept
    -> bool {
    return !(*this == o);
  }

  // in-place. unary operator- creates a new vector
  constexpr auto negate() noexcept -> Derived& {
    for (auto& component : this->components) {
      component = -component;
    }

    return *this->self();
  }

  [[nodiscard]] friend constexpr auto operator-(Derived v) noexcept -> Derived {
    v.negate();

    return v;
  }

  constexpr auto operator+=(Derived const& r) noexcept -> Derived& {
    for (auto i = uSize{0}; i < this->components.size(); ++i) {
      this->components[i] += r.components[i];
    }

    return *this->self();
  }

  [[nodiscard]] friend constexpr auto operator+(Derived l,
                                                Derived const& r) noexcept
    -> Derived {
    l += r;

    return l;
  }

  constexpr auto operator-=(Derived const& r) noexcept -> Derived& {
    for (auto i = uSize{0}; i < this->components.size(); ++i) {
      this->components[i] -= r.components[i];
    }

    return *this->self();
  }

  [[nodiscard]] friend constexpr auto operator-(Derived l,
                                                Derived const& r) noexcept
    -> Derived {
    l -= r;

    return l;
  }

  constexpr auto operator*=(T const& scalar) noexcept -> Derived& {
    for (auto& component : this->components) {
      component *= scalar;
    }

    return *this->self();
  }

  [[nodiscard]] friend constexpr auto operator*(Derived v,
                                                T const& scalar) noexcept
    -> Derived {
    v *= scalar;

    return v;
  }

  [[nodiscard]] friend constexpr auto operator*(T const& scalar,
                                                Derived const& v) noexcept
    -> Derived {
    return v * scalar;
  }

  // equivalent to (1 / scalar) * vec
  constexpr auto operator/=(T const& scalar) noexcept -> Derived& {
    for (auto& component : this->components) {
      component /= scalar;
    }

    return *this->self();
  }

  // equivalent to (1 / scalar) * vec
  [[nodiscard]] friend constexpr auto operator/(Derived v,
                                                T const& scalar) noexcept
    -> Derived {
    v /= scalar;

    return v;
  }

  [[nodiscard]]
  constexpr auto length_squared() const noexcept -> T {
    return dot(*self());
  }

  [[nodiscard]]
  auto length() const noexcept -> T;

  [[nodiscard]]
  auto normalize() const noexcept -> Derived;

  [[nodiscard]]
  constexpr auto dot(Derived const& r) const noexcept -> T {
    auto dotProduct = T{0};

    for (auto i = uSize{0}; i < this->components.size(); ++i) {
      dotProduct += this->components[i] * r.components[i];
    }

    return dotProduct;
  }

  [[nodiscard]]
  static constexpr auto dot(Derived const& l, Derived const& r) noexcept -> T {
    return l.dot(r);
  }

  [[nodiscard]]
  static auto distance(Derived const&, Derived const&) noexcept -> T;

protected:
  [[nodiscard]]
  constexpr auto self() const noexcept -> Derived const* {
    return static_cast<Derived const*>(this);
  }

  [[nodiscard]]
  constexpr auto self() noexcept -> Derived* {
    return static_cast<Derived*>(this);
  }
};

} // namespace basalt::detail
