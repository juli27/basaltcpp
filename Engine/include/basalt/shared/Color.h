#pragma once
#ifndef BASALT_COMMON_COLOR_H
#define BASALT_COMMON_COLOR_H

#include "Types.h"

namespace basalt {

struct Color final {
  constexpr Color() noexcept = default;
  constexpr Color(u8 red, u8 green, u8 blue) noexcept;
  constexpr Color(u8 red, u8 green, u8 blue, u8 alpha) noexcept;
  constexpr Color(const Color&) noexcept = default;
  constexpr Color(Color&&) noexcept = default;
  ~Color() noexcept = default;

  auto operator=(const Color&) noexcept -> Color& = default;
  auto operator=(Color&&) noexcept -> Color& = default;

  // ARGB word-order
  [[nodiscard]] constexpr auto to_argb() const noexcept -> u32;

  [[nodiscard]] constexpr auto red() const noexcept -> u8;
  [[nodiscard]] constexpr auto green() const noexcept -> u8;
  [[nodiscard]] constexpr auto blue() const noexcept -> u8;
  [[nodiscard]] constexpr auto alpha() const noexcept -> u8;

  // ARGB word order
  static constexpr auto from_argb(u32 argb) noexcept -> Color;

private:
  u8 mRed = 0;
  u8 mGreen = 0;
  u8 mBlue = 0;
  u8 mAlpha = 0;
};

constexpr Color::Color(const u8 red, const u8 green, const u8 blue) noexcept
: Color(red, green, blue, 255) {}

constexpr Color::Color(
  const u8 red, const u8 green, const u8 blue, const u8 alpha
) noexcept
: mRed(red)
, mGreen(green)
, mBlue(blue)
, mAlpha(alpha) {}

constexpr auto Color::to_argb() const noexcept -> u32 {
  u32 color = mAlpha;
  color <<= 8;
  color |= mRed;
  color <<= 8;
  color |= mGreen;
  color <<= 8;
  color |= mBlue;

  return color;
}

constexpr auto Color::red() const noexcept -> u8 {
  return mRed;
}

constexpr auto Color::green() const noexcept -> u8 {
  return mGreen;
}

constexpr auto Color::blue() const noexcept -> u8 {
  return mBlue;
}

constexpr auto Color::alpha() const noexcept -> u8 {
  return mAlpha;
}

constexpr Color Color::from_argb(const u32 argb) noexcept {
  const auto a = static_cast<u8>(argb);
  const auto r = static_cast<u8>(argb >> 8);
  const auto g = static_cast<u8>(argb >> 16);
  const auto b = static_cast<u8>(argb >> 24);

  return Color(r, g, b, a);
}

} // namespace basalt

#endif // !BASALT_COMMON_COLOR_H
