#pragma once

#include <basalt/api/base/types.h>

#include <array>

namespace basalt {

template <typename Derived, typename T, uSize Size>
struct vec {
  std::array<T, Size> elements{};

  [[nodiscard]] constexpr auto operator==(Derived const& o) const noexcept
    -> bool {
    for (auto i = uSize{0}; i < elements.size(); ++i) {
      if (elements[i] != o.elements[i]) {
        return false;
      }
    }

    return true;
  }

  [[nodiscard]] constexpr auto operator!=(Derived const& o) const noexcept
    -> bool {
    return !(*this == o);
  }

protected:
  [[nodiscard]]
  constexpr auto self() const noexcept -> Derived const* {
    return static_cast<Derived const*>(this);
  }

  [[nodiscard]] constexpr auto self() noexcept -> Derived* {
    return static_cast<Derived*>(this);
  }
};

} // namespace basalt
