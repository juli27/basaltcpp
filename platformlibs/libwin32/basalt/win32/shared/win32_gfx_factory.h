#pragma once

#include "Windows_custom.h"

#include <basalt/gfx/backend/factory.h>
#include <basalt/gfx/backend/types.h>

#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/base/types.h>

namespace basalt::gfx {

class Win32GfxFactory : public Factory {
public:
  struct DeviceAndSwapChainCreateInfo final {
    u32 adapter;
    DisplayMode exclusiveDisplayMode;
    ImageFormat renderTargetFormat{ImageFormat::Unknown};
    ImageFormat depthStencilFormat{ImageFormat::Unknown};
    MultiSampleCount sampleCount{MultiSampleCount::One};
    bool exclusive{false};
  };

  [[nodiscard]]
  auto create_context(HWND,
                      DeviceAndSwapChainCreateInfo const&) const -> ContextPtr;

protected:
  struct DeviceAndSwapChain {
    DevicePtr device;
    ext::DeviceExtensions deviceExtensions;
    SwapChainPtr swapChain;
  };

  Win32GfxFactory() noexcept = default;

  virtual auto do_create_device_and_swap_chain(
    HWND, DeviceAndSwapChainCreateInfo const&) const -> DeviceAndSwapChain = 0;
};

} // namespace basalt::gfx
