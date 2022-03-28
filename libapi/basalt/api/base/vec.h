#pragma once

#include <basalt/api/base/types.h>

#include <array>

namespace basalt {

template <typename Derived, typename T, uSize Size>
struct vec {
  std::array<T, Size> elements {};

  constexpr auto operator+=(const Derived& v) -> Derived& {
    for (uSize i = 0; i < elements.size(); ++i) {
      elements[i] += v.elements[i];
    }

    return *self();
  }

  constexpr auto operator-=(const Derived& v) -> Derived& {
    for (uSize i = 0; i < elements.size(); ++i) {
      elements[i] -= v.elements[i];
    }

    return *self();
  }

  constexpr auto operator*=(const T s) -> Derived& {
    for (uSize i = 0; i < elements.size(); ++i) {
      elements[i] *= s;
    }

    return *self();
  }

  constexpr auto operator/=(const T s) -> Derived& {
    for (uSize i = 0; i < elements.size(); ++i) {
      elements[i] /= s;
    }

    return *self();
  }

  [[nodiscard]] constexpr auto operator-() const -> Derived {
    Derived v {*self()};
    for (uSize i = 0; i < elements.size(); ++i) {
      v.elements[i] = -elements[i];
    }

    return v;
  }

  [[nodiscard]] constexpr auto operator==(const Derived& o) const -> bool {
    for (uSize i = 0; i < elements.size(); ++i) {
      if (elements[i] != o.elements[i]) {
        return false;
      }
    }

    return true;
  }

  [[nodiscard]] constexpr auto operator!=(const Derived& o) const -> bool {
    return !(*this == o);
  }

private:
  [[nodiscard]] constexpr auto self() const noexcept -> const Derived* {
    return static_cast<const Derived*>(this);
  }

  [[nodiscard]] constexpr auto self() noexcept -> Derived* {
    return static_cast<Derived*>(this);
  }

public:
  [[nodiscard]] friend constexpr auto operator+(Derived lhs, const Derived& rhs)
    -> Derived {
    lhs += rhs;
    return lhs;
  }

  [[nodiscard]] friend constexpr auto operator-(Derived l, const Derived& r)
    -> Derived {
    l -= r;
    return l;
  }

  [[nodiscard]] friend constexpr auto operator/(Derived v, const T s)
    -> Derived {
    v /= s;
    return v;
  }

  [[nodiscard]] friend constexpr auto operator*(Derived v, const T s)
    -> Derived {
    v *= s;
    return v;
  }

  [[nodiscard]] friend constexpr auto operator*(const T s, const Derived& v)
    -> Derived {
    return v * s;
  }
};

} // namespace basalt
