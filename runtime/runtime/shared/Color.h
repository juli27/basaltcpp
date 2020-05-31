#pragma once

#include "Data.h"
#include "Types.h"

namespace basalt {

struct Color final {
  // sets everything to zero
  constexpr Color() noexcept = default;

  constexpr Color(
    const f32 red, const f32 green, const f32 blue, const f32 alpha = 1.0f
  ) noexcept
    : mRed {red}, mGreen {green}, mBlue {blue}, mAlpha {alpha} {
  }

  constexpr Color(const Color&) noexcept = default;
  constexpr Color(Color&&) noexcept = default;
  ~Color() noexcept = default;

  auto operator=(const Color&) noexcept -> Color& = default;
  auto operator=(Color&&) noexcept -> Color& = default;

  // ARGB word-order
  [[nodiscard]]
  constexpr auto to_argb() const noexcept -> ColorEncoding::A8R8G8B8 {
    const auto red = static_cast<u8>(mRed * 255.0f);
    const auto green = static_cast<u8>(mGreen * 255.0f);
    const auto blue = static_cast<u8>(mBlue * 255.0f);
    const auto alpha = static_cast<u8>(mAlpha * 255.0f);

    return ColorEncoding::pack_logical_a8r8g8b8(red, green, blue, alpha);
  }

  [[nodiscard]]
  constexpr auto red() const noexcept -> f32 {
    return mRed;
  }

  [[nodiscard]]
  constexpr auto green() const noexcept -> f32 {
    return mGreen;
  }

  [[nodiscard]]
  constexpr auto blue() const noexcept -> f32 {
    return mBlue;
  }

  [[nodiscard]]
  constexpr auto alpha() const noexcept -> f32 {
    return mAlpha;
  }

  [[nodiscard]]
  static constexpr auto from_rgba(
    const u8 red, const u8 green, const u8 blue, const u8 alpha = 255
  ) noexcept -> Color {
    return Color {
      static_cast<float>(red) / 255.0f, static_cast<float>(green) / 255.0f
    , static_cast<float>(blue) / 255.0f, static_cast<float>(alpha) / 255.0f
    };
  }

  // ARGB word order
  [[nodiscard]]
  static constexpr auto from(const ColorEncoding::A8R8G8B8 argb) noexcept -> Color {
    const auto data = static_cast<u32>(argb);
    const auto a = static_cast<u8>(data);
    const auto r = static_cast<u8>(data >> 8);
    const auto g = static_cast<u8>(data >> 16);
    const auto b = static_cast<u8>(data >> 24);

    return from_rgba(r, g, b, a);
  }

private:
  f32 mRed {0};
  f32 mGreen {0};
  f32 mBlue {0};
  f32 mAlpha {0};
};

struct Colors final {
  static constexpr Color BLACK {0.0f, 0.0f, 0.0f};
  static constexpr Color WHITE {1.0f, 1.0f, 1.0f};
  static constexpr Color RED {1.0f, 0.0f, 0.0f};
  static constexpr Color GREEN {0.0f, 1.0f, 0.0f};
  static constexpr Color BLUE {0.0f, 0.0f, 1.0f};
};

} // namespace basalt
