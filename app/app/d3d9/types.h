#pragma once

#include <runtime/shared/Types.h>

#include <string>
#include <vector>

namespace basalt::gfx::backend {

// listed from left to right, in c-array order if power of 2 aligned,
// lsb to msb otherwise
enum class SurfaceFormat : u8 {
  // 16-bit
  B5G6R5
, B5G5R5X1
, B5G5R5A1

  // 32-bit
, B8G8R8X8
, B8G8R8A8
, B10G10R10A2
};

struct AdapterMode final {
  u32 width {};
  u32 height {};
  u32 refreshRate {};
  SurfaceFormat displayFormat {};
};

struct AdapterInfo final {
  std::string displayName {};
  std::string driver {};
  std::string driverVersion {};
  std::vector<AdapterMode> adapterModes {};
  AdapterMode defaultAdapterMode {};
};

} // namespace basalt::gfx::backend
