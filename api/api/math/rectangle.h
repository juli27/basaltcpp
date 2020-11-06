#pragma once

#include "api/base/vec.h"

namespace basalt {

template <typename T>
struct Rectangle final : vec<Rectangle<T>, T, 4> {
  constexpr Rectangle() noexcept = default;

  constexpr Rectangle(T left, T top, T right, T bottom) noexcept
    : vec {left, top, right, bottom} {
  }

  [[nodiscard]] auto left() const -> T {
    return std::get<0>(elements);
  }

  [[nodiscard]] auto top() const -> T {
    return std::get<1>(elements);
  }

  [[nodiscard]] auto right() const -> T {
    return std::get<2>(elements);
  }

  [[nodiscard]] auto bottom() const -> T {
    return std::get<3>(elements);
  }

  [[nodiscard]] auto area() const -> T {
    return (right() - left()) * (bottom() - top());
  }
};

using RectangleU16 = Rectangle<u16>;

} // namespace basalt
