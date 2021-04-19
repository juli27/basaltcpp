#pragma once

#include <basalt/api/base/types.h>

#include <array>

namespace basalt {

template <typename Derived, typename T, uSize Size>
struct vec {
  constexpr vec() noexcept = default;

  std::array<T, Size> elements {};

  auto operator+=(const vec& v) noexcept -> Derived& {
    for (uSize i = 0; i < elements.size(); ++i) {
      elements[i] += v.elements[i];
    }

    return *self();
  }

  auto operator-=(const vec& v) noexcept -> Derived& {
    for (uSize i = 0; i < elements.size(); ++i) {
      elements[i] -= v.elements[i];
    }

    return *self();
  }

  auto operator*=(const T s) noexcept -> Derived& {
    for (uSize i = 0; i < elements.size(); ++i) {
      elements[i] *= s;
    }

    return *self();
  }

  auto operator/=(const T s) noexcept -> Derived& {
    for (uSize i = 0; i < elements.size(); ++i) {
      elements[i] /= s;
    }

    return *self();
  }

  constexpr auto operator-() const noexcept -> Derived {
    Derived v;
    for (uSize i = 0; i < elements.size(); ++i) {
      v.elements[i] = -elements[i];
    }

    return v;
  }

  [[nodiscard]] auto operator==(const Derived& o) const noexcept -> bool {
    for (uSize i = 0; i < elements.size(); ++i) {
      if (elements[i] != o.elements[i]) {
        return false;
      }
    }

    return true;
  }

  [[nodiscard]] auto operator!=(const Derived& o) const noexcept -> bool {
    return !(*this == o);
  }

private:
  [[nodiscard]] auto self() noexcept -> Derived* {
    return static_cast<Derived*>(this);
  }

public:
  friend auto operator+(Derived lhs, const Derived& rhs) noexcept -> Derived {
    lhs += rhs;
    return lhs;
  }

  friend auto operator-(Derived l, const Derived& r) noexcept -> Derived {
    l -= r;
    return l;
  }

  friend auto operator/(Derived v, const T s) noexcept -> Derived {
    v /= s;
    return v;
  }

  friend auto operator*(Derived v, const T s) noexcept -> Derived {
    v *= s;
    return v;
  }

  friend auto operator*(const T s, const Derived& v) noexcept -> Derived {
    return v * s;
  }
};

} // namespace basalt
