#pragma once

#include <basalt/api/base/vec.h>

#include <basalt/api/math/rectangle.h>

namespace basalt {

template <typename T>
class Size2D final : public vec<Size2D<T>, T, 2> {
public:
  constexpr Size2D() noexcept = default;

  constexpr Size2D(const T width, const T height) noexcept
    : vec {width, height} {
  }

  [[nodiscard]] constexpr auto width() const noexcept -> T {
    return std::get<0>(this->elements);
  }

  [[nodiscard]] constexpr auto height() const noexcept -> T {
    return std::get<1>(this->elements);
  }

  constexpr auto set(const T width, const T height) noexcept -> void {
    std::get<0>(this->elements) = width;
    std::get<1>(this->elements) = height;
  }

  constexpr auto set_width(const T width) noexcept -> void {
    std::get<0>(this->elements) = width;
  }

  constexpr auto set_height(const T height) noexcept -> void {
    std::get<1>(this->elements) = height;
  }

  [[nodiscard]] constexpr auto to_rectangle() const noexcept -> Rectangle<T> {
    return Rectangle<T> {0, 0, width(), height()};
  }

  // x : 1 (width : height)
  [[nodiscard]] constexpr auto aspect_ratio() const noexcept -> f32 {
    return static_cast<f32>(width()) / static_cast<f32>(height());
  }

  [[nodiscard]] static constexpr auto dont_care() noexcept -> Size2D {
    return Size2D {};
  }
};

} // namespace basalt
