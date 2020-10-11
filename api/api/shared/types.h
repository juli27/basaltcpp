#pragma once

#include "api/base/types.h"

namespace basalt {

struct Color;
struct Config;

template <typename T>
struct Size2D;
using Size2Du16 = Size2D<u16>;

enum class WindowMode : u8 {
  Windowed,
  Fullscreen,
  FullscreenExclusive,
};

} // namespace basalt
