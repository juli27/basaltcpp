#pragma once

#include <basalt/api/shared/data.h>

#include <basalt/api/base/vec.h>

namespace basalt {

// TODO: this should be in linear representation (convert linear <-> non-linear)
// TODO: clamp to [0.0f,1.0f] within class / when converting
// non-linear sRGB color
struct Color final : vec<Color, f32, 4> {
  constexpr Color() noexcept = default;

  // TODO: when class represents linearly this should take in linear values
  // input is non-linear
  constexpr Color(const f32 red, const f32 green, const f32 blue,
                  const f32 alpha = 1.0f) noexcept
    : vec {red, green, blue, alpha} {
  }

  // output is non-linear
  [[nodiscard]] constexpr auto to_argb() const noexcept
    -> ColorEncoding::A8R8G8B8 {
    const auto r = static_cast<u8>(red() * 255.0f);
    const auto g = static_cast<u8>(green() * 255.0f);
    const auto b = static_cast<u8>(blue() * 255.0f);
    const auto a = static_cast<u8>(alpha() * 255.0f);

    return ColorEncoding::pack_a8r8g8b8_u32(r, g, b, a);
  }

  [[nodiscard]] constexpr auto red() const noexcept -> f32 {
    return std::get<0>(elements);
  }

  [[nodiscard]] constexpr auto green() const noexcept -> f32 {
    return std::get<1>(elements);
  }

  [[nodiscard]] constexpr auto blue() const noexcept -> f32 {
    return std::get<2>(elements);
  }

  [[nodiscard]] constexpr auto alpha() const noexcept -> f32 {
    return std::get<3>(elements);
  }

  // input is non-linear
  [[nodiscard]] static constexpr auto from_rgba(const u8 red, const u8 green,
                                                const u8 blue,
                                                const u8 alpha = 255) noexcept
    -> Color {
    return Color {
      static_cast<f32>(red) / 255.0f, static_cast<f32>(green) / 255.0f,
      static_cast<f32>(blue) / 255.0f, static_cast<f32>(alpha) / 255.0f};
  }

  // input is non-linear
  [[nodiscard]] static constexpr auto
  from(const ColorEncoding::A8R8G8B8 argb) noexcept -> Color {
    const auto [r, g, b, a] = ColorEncoding::unpack(argb);

    return from_rgba(r, g, b, a);
  }
};

struct Colors final {
  static constexpr Color BLACK {0.0f, 0.0f, 0.0f};
  static constexpr Color WHITE {1.0f, 1.0f, 1.0f};
  static constexpr Color RED {1.0f, 0.0f, 0.0f};
  static constexpr Color GREEN {0.0f, 1.0f, 0.0f};
  static constexpr Color BLUE {0.0f, 0.0f, 1.0f};
  static constexpr Color YELLOW {1.0f, 1.0f, 0.0f};
};

} // namespace basalt
