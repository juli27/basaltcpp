#pragma once

#include <basalt/api/math/types.h>

#include <basalt/api/base/types.h>

#include <array>

namespace basalt::detail {

template <typename Derived, typename T, uSize Size>
struct Vector {
  // this must be an aggregate type

  [[nodiscard]] static constexpr auto zero() noexcept -> Derived {
    return Derived {};
  }

  [[nodiscard]] static auto normalize(Derived v) noexcept -> Derived;

  std::array<T, Size> components {};

  [[nodiscard]] constexpr auto operator==(const Derived& o) const noexcept
    -> bool {
    for (uSize i {0}; i < components.size(); ++i) {
      if (components[i] != o.components[i]) {
        return false;
      }
    }

    return true;
  }

  [[nodiscard]] constexpr auto operator!=(const Derived& o) const noexcept
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

  constexpr auto operator+=(const Derived& r) noexcept -> Derived& {
    for (uSize i {0}; i < this->components.size(); ++i) {
      this->components[i] += r.components[i];
    }

    return *this->self();
  }

  [[nodiscard]] friend constexpr auto operator+(Derived l,
                                                const Derived& r) noexcept
    -> Derived {
    l += r;

    return l;
  }

  constexpr auto operator-=(const Derived& r) noexcept -> Derived& {
    for (uSize i {0}; i < this->components.size(); ++i) {
      this->components[i] -= r.components[i];
    }

    return *this->self();
  }

  [[nodiscard]] friend constexpr auto operator-(Derived l,
                                                const Derived& r) noexcept
    -> Derived {
    l -= r;

    return l;
  }

  constexpr auto operator*=(const T& scalar) noexcept -> Derived& {
    for (auto& component : this->components) {
      component *= scalar;
    }

    return *this->self();
  }

  [[nodiscard]] friend constexpr auto operator*(Derived v,
                                                const T& scalar) noexcept
    -> Derived {
    v *= scalar;

    return v;
  }

  [[nodiscard]] friend constexpr auto operator*(const T& scalar,
                                                const Derived& v) noexcept
    -> Derived {
    return v * scalar;
  }

  // equivalent to (1 / scalar) * vec
  constexpr auto operator/=(const T& scalar) noexcept -> Derived& {
    for (auto& component : this->components) {
      component /= scalar;
    }

    return *this->self();
  }

  // equivalent to (1 / scalar) * vec
  [[nodiscard]] friend constexpr auto operator/(Derived v,
                                                const T& scalar) noexcept
    -> Derived {
    v /= scalar;

    return v;
  }

  [[nodiscard]] constexpr auto length_squared() const noexcept -> T {
    return dot(*self());
  }

  [[nodiscard]] auto length() const noexcept -> T;

  auto normalize() noexcept -> void;

  [[nodiscard]] constexpr auto dot(const Derived& r) const noexcept -> T {
    T dotProduct {0};

    for (uSize i {0}; i < this->components.size(); ++i) {
      dotProduct += this->components[i] * r.components[i];
    }

    return dotProduct;
  }

  [[nodiscard]] static constexpr auto dot(const Derived& l,
                                          const Derived& r) noexcept -> T {
    return l.dot(r);
  }

  [[nodiscard]] static auto distance(const Derived&, const Derived&) noexcept
    -> T;

protected:
  [[nodiscard]] constexpr auto self() const noexcept -> const Derived* {
    return static_cast<const Derived*>(this);
  }

  [[nodiscard]] constexpr auto self() noexcept -> Derived* {
    return static_cast<Derived*>(this);
  }
};

} // namespace basalt::detail
