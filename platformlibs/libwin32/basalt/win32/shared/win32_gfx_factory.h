#pragma once

#include <basalt/win32/shared/Windows_custom.h>

#include <basalt/gfx/backend/types.h>

#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>

namespace basalt::gfx {

class Win32GfxFactory {
public:
  struct DeviceAndSwapChainDesc final {
    Adapter adapter;
    DisplayMode exclusiveDisplayMode;
    ImageFormat renderTargetFormat {ImageFormat::Unknown};
    ImageFormat depthStencilFormat {ImageFormat::Unknown};
    MultiSampleCount sampleCount {MultiSampleCount::One};
    bool exclusive {false};
  };

  struct DeviceAndSwapChain final {
    DevicePtr device;
    SwapChainPtr swapChain;
  };

  Win32GfxFactory(const Win32GfxFactory&) = delete;
  Win32GfxFactory(Win32GfxFactory&&) = delete;

  virtual ~Win32GfxFactory() noexcept = default;

  auto operator=(const Win32GfxFactory&) -> Win32GfxFactory& = delete;
  auto operator=(Win32GfxFactory&&) -> Win32GfxFactory& = delete;

  [[nodiscard]] virtual auto adapters() const -> const AdapterList& = 0;

  auto create_device_and_swap_chain(HWND, const DeviceAndSwapChainDesc&) const
    -> DeviceAndSwapChain;

protected:
  Win32GfxFactory() noexcept = default;

  virtual auto
  do_create_device_and_swap_chain(HWND, const DeviceAndSwapChainDesc&) const
    -> DeviceAndSwapChain = 0;
};

} // namespace basalt::gfx
