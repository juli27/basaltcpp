#pragma once

#include <basalt/api/base/types.h>

#include <string>
#include <vector>

namespace basalt::gfx {

// listed from left to right, in c-array order if power of 2 aligned,
// lsb to msb otherwise
enum class ImageFormat : u8 {
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

struct AdapterMode final {
  u32 width {};
  u32 height {};
  u32 refreshRate {};
  ImageFormat displayFormat {};
};

using AdapterModeList = std::vector<AdapterMode>;

struct AdapterCapabilities final {};

struct AdapterInfo final {
  std::string displayName {};
  std::string driverInfo {};
  AdapterModeList adapterModes {};
  u32 adapterIndex {0};
  AdapterCapabilities capabilities {};
};

using AdapterList = std::vector<AdapterInfo>;

struct Info final {
  AdapterList adapters {};
};

} // namespace basalt::gfx
