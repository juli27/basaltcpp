#pragma once

#include <basalt/api/base/types.h>
#include <basalt/api/base/utils.h>

namespace basalt {

class ColorEncoding final {
public:
  // logical order (big-endian: a r g b, little-endian: b g r a)
  enum class A8R8G8B8 : u32 {};

  struct RGBA final {
    u8 r {};
    u8 g {};
    u8 b {};
    u8 a {};
  };

  [[nodiscard]] static constexpr auto
  pack_a8r8g8b8_u32(const u8 r, const u8 g, const u8 b,
                    const u8 a = 255) noexcept -> A8R8G8B8 {
    auto packed {static_cast<u32>(a) << 24};
    packed |= static_cast<u32>(r) << 16;
    packed |= static_cast<u32>(g) << 8;
    packed |= static_cast<u32>(b);

    return A8R8G8B8 {packed};
  }

  [[nodiscard]] static constexpr auto unpack(const A8R8G8B8 argb) noexcept
    -> RGBA {
    const u32 data {enum_cast(argb)};
    const auto a {static_cast<u8>(data)};
    const auto r {static_cast<u8>(data >> 8)};
    const auto g {static_cast<u8>(data >> 16)};
    const auto b {static_cast<u8>(data >> 24)};

    return RGBA {r, g, b, a};
  }
};

inline namespace literals {

[[nodiscard]] constexpr auto
operator"" _a8r8g8b8(const unsigned long long color) noexcept
  -> ColorEncoding::A8R8G8B8 {
  return ColorEncoding::A8R8G8B8 {static_cast<u32>(color)};
}

} // namespace literals

} // namespace basalt
