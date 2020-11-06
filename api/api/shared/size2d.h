#pragma once

#include "api/base/vec.h"

#include "api/math/rectangle.h"

namespace basalt {

template <typename T>
struct Size2D final : vec<Size2D<T>, T, 2> {
  constexpr Size2D() noexcept = default;

  constexpr Size2D(const T width, const T height) noexcept
    : vec {width, height} {
  }

  [[nodiscard]] constexpr auto width() const noexcept -> T {
    return std::get<0>(elements);
  }

  [[nodiscard]] constexpr auto height() const noexcept -> T {
    return std::get<1>(elements);
  }

  constexpr void set(const T width, const T height) noexcept {
    std::get<0>(elements) = width;
    std::get<1>(elements) = height;
  }

  constexpr void set_width(const T width) noexcept {
    std::get<0>(elements) = width;
  }

  constexpr void set_height(const T height) noexcept {
    std::get<1>(elements) = height;
  }

  [[nodiscard]] constexpr auto to_rectangle() const noexcept -> Rectangle<T> {
    return Rectangle<T> {0, 0, width(), height()};
  }

  [[nodiscard]] static constexpr auto dont_care() noexcept -> Size2D {
    return Size2D {};
  }
};

using Size2Du16 = Size2D<u16>;

} // namespace basalt
