#pragma once

#include "types.h"

namespace basalt {

template <typename T>
struct Size2D final {
  constexpr Size2D(const T width, const T height) noexcept
    : mWidth {width}, mHeight {height} {
  }

  [[nodiscard]] constexpr auto width() const noexcept -> T {
    return mWidth;
  }

  [[nodiscard]] constexpr auto height() const noexcept -> T {
    return mHeight;
  }

  constexpr void set(const T width, const T height) noexcept {
    mWidth = width;
    mHeight = height;
  }

  constexpr void set_width(const T width) noexcept {
    mWidth = width;
  }

  constexpr void set_height(const T height) noexcept {
    mHeight = height;
  }

  friend auto operator==(const Size2D& lhs, const Size2D& rhs) noexcept
    -> bool {
    return lhs.mWidth == rhs.mWidth && lhs.mHeight == rhs.mHeight;
  }

  friend auto operator!=(const Size2D& lhs, const Size2D& rhs) noexcept
    -> bool {
    return !(lhs == rhs);
  }

private:
  T mWidth {};
  T mHeight {};

  constexpr Size2D() noexcept = default;

public:
  [[nodiscard]] static constexpr auto dont_care() noexcept -> Size2D {
    return Size2D {};
  }
};

using Size2Du16 = Size2D<u16>;

} // namespace basalt
