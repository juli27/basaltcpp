#pragma once

#include <basalt/api/base/types.h>

namespace basalt {

struct ColorEncoding final {
  // logical order (big-endian: a r g b, little-endian: b g r a)
  enum class A8R8G8B8 : u32 {};

  struct RGBA final {
    u8 r {};
    u8 g {};
    u8 b {};
    u8 a {};
  };

  [[nodiscard]] static constexpr auto
  pack_a8r8g8b8_u32(const u8 red, const u8 green, const u8 blue,
                    const u8 alpha = 255) noexcept -> A8R8G8B8 {
    auto packed {static_cast<u32>(alpha) << 24};
    packed |= static_cast<u32>(red) << 16;
    packed |= static_cast<u32>(green) << 8;
    packed |= static_cast<u32>(blue);

    return A8R8G8B8 {packed};
  }

  [[nodiscard]] static constexpr auto unpack(const A8R8G8B8 argb) noexcept
    -> RGBA {
    const auto data = static_cast<u32>(argb);
    const auto a = static_cast<u8>(data);
    const auto r = static_cast<u8>(data >> 8);
    const auto g = static_cast<u8>(data >> 16);
    const auto b = static_cast<u8>(data >> 24);

    return RGBA {r, g, b, a};
  }
};

} // namespace basalt
