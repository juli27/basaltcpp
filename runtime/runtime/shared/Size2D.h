#pragma once
#ifndef BASALT_MATH_SIZE_2D_H
#define BASALT_MATH_SIZE_2D_H

#include "runtime/shared/Types.h"

namespace basalt {

template <typename T>
struct Size2D final {
  constexpr Size2D(T width, T height);

  constexpr Size2D(const Size2D&) = default;
  constexpr Size2D(Size2D&&) = default;

  ~Size2D() = default;

  auto operator=(const Size2D& other) -> Size2D& = default;
  auto operator=(Size2D&& other) -> Size2D& = default;

  constexpr void set(T width, T height);
  constexpr void set_width(T width);
  constexpr void set_height(T height);

  [[nodiscard]]
  constexpr auto width() const -> T;
  [[nodiscard]]
  constexpr auto height() const -> T;

  friend auto operator==(const Size2D& lhs, const Size2D& rhs) -> bool {
    return lhs.mWidth == rhs.mWidth
      && lhs.mHeight == rhs.mHeight;
  }

  friend auto operator!=(const Size2D& lhs, const Size2D& rhs) -> bool {
    return !(lhs == rhs);
  }

private:
  constexpr Size2D() = default;

public:
  [[nodiscard]] static constexpr auto dont_care() -> Size2D;

private:
  T mWidth {};
  T mHeight {};
};


template <typename T>
constexpr Size2D<T>::Size2D(T width, T height)
  : mWidth {width}
  , mHeight {height} {
}

template <typename T>
constexpr void Size2D<T>::set(T width, T height) {
  mWidth = width;
  mHeight = height;
}

template <typename T>
constexpr void Size2D<T>::set_width(T width) {
  mWidth = width;
}

template <typename T>
constexpr void Size2D<T>::set_height(T height) {
  mHeight = height;
}

template <typename T>
constexpr auto Size2D<T>::width() const -> T {
  return this->mWidth;
}

template <typename T>
constexpr auto Size2D<T>::height() const -> T {
  return this->mHeight;
}

template <typename T>
auto constexpr Size2D<T>::dont_care() -> Size2D {
  return {};
}

using Size2Du16 = Size2D<u16>;

} // namespace basalt

#endif // BASALT_MATH_SIZE_2D_H
