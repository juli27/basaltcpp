#pragma once

#include <basalt/api/base/types.h>

namespace basalt::gfx {

// listed from left to right, in c-array order if power of 2 aligned,
// lsb to msb otherwise
enum class ImageFormat : u8
{
  Unknown,

  // 16-bit
  B5G6R5,
  B5G5R5X1,
  B5G5R5A1,

  // 32-bit
  B8G8R8X8,
  B8G8R8A8,
  B10G10R10A2,
};

} // namespace basalt::gfx
