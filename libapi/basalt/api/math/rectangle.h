#pragma once

#include <basalt/api/base/vec.h>

namespace basalt {

template <typename T>
struct Rectangle final : vec<Rectangle<T>, T, 4> {
  constexpr Rectangle() noexcept = default;

  constexpr Rectangle(T left, T top, T right, T bottom) noexcept
    : vec {left, top, right, bottom} {
  }

  [[nodiscard]] auto left() const -> T {
    return std::get<0>(this->elements);
  }

  [[nodiscard]] auto top() const -> T {
    return std::get<1>(this->elements);
  }

  [[nodiscard]] auto right() const -> T {
    return std::get<2>(this->elements);
  }

  [[nodiscard]] auto bottom() const -> T {
    return std::get<3>(this->elements);
  }

  [[nodiscard]] auto width() const -> T {
    return right() - left();
  }

  [[nodiscard]] auto height() const -> T {
    return bottom() - top();
  }

  [[nodiscard]] auto area() const -> T {
    return (right() - left()) * (bottom() - top());
  }
};

using RectangleI16 = Rectangle<i16>;
using RectangleU16 = Rectangle<u16>;

} // namespace basalt
