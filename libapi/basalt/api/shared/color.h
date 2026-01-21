#pragma once

#include <basalt/api/shared/data.h>

#include <basalt/api/math/vector_p.h>

#include <algorithm>

namespace basalt {

// TODO: tbColorNegate, tbColorBrightness, tbColorRandom
// TODO: tbColorInterpolate, tbColorMin, tbColorMax
// TODO: is this really a vector (in the math sense)

// TODO: this should be in linear representation (convert linear <-> non-linear)
// non-linear sRGB color
struct Color final : detail::Vector<Color, f32, 4> {
  // input is non-linear
  [[nodiscard]]
  static constexpr auto from(ColorEncoding::A8R8G8B8 const argb) noexcept
    -> Color {
    auto const [r, g, b, a] = ColorEncoding::unpack(argb);

    return from_non_linear_rgba8(r, g, b, a);
  }

  // input is non-linear
  [[nodiscard]]
  static constexpr auto from_non_linear_rgba8(u8 const r, u8 const g,
                                              u8 const b,
                                              u8 const a = 255) noexcept
    -> Color {
    return from_non_linear(
      static_cast<f32>(r) / 255.0f, static_cast<f32>(g) / 255.0f,
      static_cast<f32>(b) / 255.0f, static_cast<f32>(a) / 255.0f);
  }

  [[nodiscard]]
  static constexpr auto from_non_linear(f32 const r, f32 const g, f32 const b,
                                        f32 const a = 1.0f) noexcept -> Color {
    return Color{r, g, b, a};
  }

  constexpr Color() noexcept = default;

  [[nodiscard]]
  constexpr auto r() const noexcept -> f32 {
    return get<0>();
  }

  [[nodiscard]]
  constexpr auto g() const noexcept -> f32 {
    return get<1>();
  }

  [[nodiscard]]
  constexpr auto b() const noexcept -> f32 {
    return get<2>();
  }

  [[nodiscard]]
  constexpr auto a() const noexcept -> f32 {
    return get<3>();
  }

  // output is non-linear
  [[nodiscard]]
  constexpr auto to_argb() const noexcept -> ColorEncoding::A8R8G8B8 {
    auto const r = static_cast<u8>(std::clamp(this->r(), 0.0f, 1.0f) * 255.0f);
    auto const g = static_cast<u8>(std::clamp(this->g(), 0.0f, 1.0f) * 255.0f);
    auto const b = static_cast<u8>(std::clamp(this->b(), 0.0f, 1.0f) * 255.0f);
    auto const a = static_cast<u8>(std::clamp(this->a(), 0.0f, 1.0f) * 255.0f);

    return ColorEncoding::pack_a8r8g8b8_u32(r, g, b, a);
  }

private:
  // TODO: when class represents linearly this should take in linear values
  // input is non-linear
  constexpr Color(f32 const r, f32 const g, f32 const b,
                  f32 const a = 1.0f) noexcept
    : Vector{r, g, b, a} {
  }
};

struct Colors final {
  static constexpr auto BLACK = Color::from_non_linear(0.0f, 0.0f, 0.0f);
  static constexpr auto WHITE = Color::from_non_linear(1.0f, 1.0f, 1.0f);
  static constexpr auto RED = Color::from_non_linear(1.0f, 0.0f, 0.0f);
  static constexpr auto GREEN = Color::from_non_linear(0.0f, 1.0f, 0.0f);
  static constexpr auto BLUE = Color::from_non_linear(0.0f, 0.0f, 1.0f);
  static constexpr auto YELLOW = Color::from_non_linear(1.0f, 1.0f, 0.0f);
};

} // namespace basalt
