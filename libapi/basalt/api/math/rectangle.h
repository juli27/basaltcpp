#pragma once

#include <basalt/api/base/vec.h>

#include <array>

namespace basalt {

template <typename T>
struct Rectangle final : vec<Rectangle<T>, T, 4> {
  constexpr Rectangle() noexcept = default;

  constexpr Rectangle(T left, T top, T right, T bottom) noexcept
    : vec {left, top, right, bottom} {
  }

  [[nodiscard]] constexpr auto left() const noexcept -> T {
    return std::get<0>(this->elements);
  }

  [[nodiscard]] constexpr auto top() const noexcept -> T {
    return std::get<1>(this->elements);
  }

  [[nodiscard]] constexpr auto right() const noexcept -> T {
    return std::get<2>(this->elements);
  }

  [[nodiscard]] constexpr auto bottom() const noexcept -> T {
    return std::get<3>(this->elements);
  }

  [[nodiscard]] constexpr auto width() const noexcept -> T {
    return right() - left();
  }

  [[nodiscard]] constexpr auto height() const noexcept -> T {
    return bottom() - top();
  }

  [[nodiscard]] constexpr auto area() const noexcept -> T {
    return (right() - left()) * (bottom() - top());
  }
};

} // namespace basalt
