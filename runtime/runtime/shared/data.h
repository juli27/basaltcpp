#pragma once

#include "types.h"

namespace basalt {

struct ColorEncoding final {
  // logical order (big-endian: a r g b, little-endian: b g r a)
  enum class A8R8G8B8 : u32 {
  };

  [[nodiscard]]
  static constexpr auto pack_logical_a8r8g8b8(
    const u8 red, const u8 green, const u8 blue, const u8 alpha = 255
  ) noexcept -> A8R8G8B8 {
    u32 pixel {static_cast<u32>(alpha) << 24};
    pixel |= static_cast<u32>(red) << 16;
    pixel |= static_cast<u32>(green) << 8;
    pixel |= static_cast<u32>(blue);

    return A8R8G8B8 {pixel};
  }
};

} // namespace basalt
