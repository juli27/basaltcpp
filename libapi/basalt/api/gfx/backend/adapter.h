#pragma once

#include "types.h"

#include "basalt/api/base/enum_set.h"
#include "basalt/api/base/types.h"

#include <string>
#include <vector>

namespace basalt::gfx {

using MultiSampleCounts = EnumSet<MultiSampleCount, MultiSampleCount::Sixteen>;

struct BackBufferFormat {
  ImageFormat renderTargetFormat;
  ImageFormat depthStencilFormat;
  MultiSampleCounts supportedSampleCounts;
};

using BackBufferFormats = std::vector<BackBufferFormat>;

struct DisplayMode {
  u32 width;
  u32 height;
  u32 refreshRate;
};

using DisplayModes = std::vector<DisplayMode>;

struct AdapterSharedModeInfo {
  BackBufferFormats backBufferFormats;
  DisplayMode displayMode;
  ImageFormat displayFormat;
};

struct AdapterExclusiveModeInfo {
  BackBufferFormats backBufferFormats;
  DisplayModes displayModes;
  ImageFormat displayFormat;
};

using AdapterExclusiveModeInfos = std::vector<AdapterExclusiveModeInfo>;

struct PciId {
  u16 vendorId;
  u16 deviceId;
  u8 revision;
  u16 subsystemVendorId;
  u16 subsystemId;
};

struct AdapterIdentifier {
  std::string id;
  std::string displayName;
  std::string driverInfo;
  PciId pciId;
};

struct AdapterInfo {
  AdapterSharedModeInfo sharedModeInfo;
  AdapterExclusiveModeInfos exclusiveModes;
  AdapterIdentifier identifier;
  DeviceCaps deviceCaps;
  u32 index;
};

using AdapterInfos = std::vector<AdapterInfo>;

} // namespace basalt::gfx
