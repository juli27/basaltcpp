#pragma once

#include <basalt/api/base/types.h>

#include <array>

namespace basalt {

template <typename Derived, typename T, uSize Size>
struct vec {
  std::array<T, Size> elements {};

  [[nodiscard]] constexpr auto operator==(const Derived& o) const noexcept
    -> bool {
    for (uSize i {0}; i < elements.size(); ++i) {
      if (elements[i] != o.elements[i]) {
        return false;
      }
    }

    return true;
  }

  [[nodiscard]] constexpr auto operator!=(const Derived& o) const noexcept
    -> bool {
    return !(*this == o);
  }

protected:
  [[nodiscard]] constexpr auto self() const noexcept -> const Derived* {
    return static_cast<const Derived*>(this);
  }

  [[nodiscard]] constexpr auto self() noexcept -> Derived* {
    return static_cast<Derived*>(this);
  }
};

} // namespace basalt
