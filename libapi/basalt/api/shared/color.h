#pragma once

#include <basalt/api/shared/data.h>

#include <basalt/api/math/vector.h>

#include <algorithm>

namespace basalt {

// TODO: tbColorNegate, tbColorBrightness, tbColorRandom
// TODO: tbColorInterpolate, tbColorMin, tbColorMax
// TODO: is this really a vector (in the math sense)

// TODO: this should be in linear representation (convert linear <-> non-linear)
// non-linear sRGB color
struct Color final : detail::Vector<Color, f32, 4> {
  // input is non-linear
  [[nodiscard]] static constexpr auto
  from(const ColorEncoding::A8R8G8B8 argb) noexcept -> Color {
    const auto [r, g, b, a] {ColorEncoding::unpack(argb)};

    return from_non_linear_rgba8(r, g, b, a);
  }

  // input is non-linear
  [[nodiscard]] static constexpr auto
  from_non_linear_rgba8(const u8 r, const u8 g, const u8 b,
                        const u8 a = 255) noexcept -> Color {
    return from_non_linear(
      static_cast<f32>(r) / 255.0f, static_cast<f32>(g) / 255.0f,
      static_cast<f32>(b) / 255.0f, static_cast<f32>(a) / 255.0f);
  }

  [[nodiscard]] static constexpr auto
  from_non_linear(const f32 r, const f32 g, const f32 b,
                  const f32 a = 1.0f) noexcept -> Color {
    return Color {r, g, b, a};
  }

  constexpr Color() noexcept = default;

  [[nodiscard]] constexpr auto r() const noexcept -> f32 {
    return std::get<0>(components);
  }

  [[nodiscard]] constexpr auto g() const noexcept -> f32 {
    return std::get<1>(components);
  }

  [[nodiscard]] constexpr auto b() const noexcept -> f32 {
    return std::get<2>(components);
  }

  [[nodiscard]] constexpr auto a() const noexcept -> f32 {
    return std::get<3>(components);
  }

  // output is non-linear
  [[nodiscard]] constexpr auto to_argb() const noexcept
    -> ColorEncoding::A8R8G8B8 {
    const auto r {static_cast<u8>(std::clamp(this->r(), 0.0f, 1.0f) * 255.0f)};
    const auto g {static_cast<u8>(std::clamp(this->g(), 0.0f, 1.0f) * 255.0f)};
    const auto b {static_cast<u8>(std::clamp(this->b(), 0.0f, 1.0f) * 255.0f)};
    const auto a {static_cast<u8>(std::clamp(this->a(), 0.0f, 1.0f) * 255.0f)};

    return ColorEncoding::pack_a8r8g8b8_u32(r, g, b, a);
  }

private:
  // TODO: when class represents linearly this should take in linear values
  // input is non-linear
  constexpr Color(const f32 r, const f32 g, const f32 b,
                  const f32 a = 1.0f) noexcept
    : Vector {r, g, b, a} {
  }
};

struct Colors final {
  static constexpr Color BLACK {Color::from_non_linear(0.0f, 0.0f, 0.0f)};
  static constexpr Color WHITE {Color::from_non_linear(1.0f, 1.0f, 1.0f)};
  static constexpr Color RED {Color::from_non_linear(1.0f, 0.0f, 0.0f)};
  static constexpr Color GREEN {Color::from_non_linear(0.0f, 1.0f, 0.0f)};
  static constexpr Color BLUE {Color::from_non_linear(0.0f, 0.0f, 1.0f)};
  static constexpr Color YELLOW {Color::from_non_linear(1.0f, 1.0f, 0.0f)};
};

} // namespace basalt
