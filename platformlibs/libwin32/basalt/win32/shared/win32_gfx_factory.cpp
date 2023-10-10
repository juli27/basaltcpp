#include <basalt/win32/shared/win32_gfx_factory.h>

#include <basalt/gfx/backend/device.h>

#include <basalt/api/gfx/context.h>

#include <utility>

namespace basalt::gfx {

auto Win32GfxFactory::create_context(HWND const window,
                                     DeviceAndSwapChainDesc const& desc) const
  -> ContextPtr {
  auto [device, swapChain] = do_create_device_and_swap_chain(window, desc);

  auto info = Info{
    device->capabilities(),
    adapters(),
    BackendApi::Direct3D9,
  };

  return Context::create(std::move(device), std::move(swapChain),
                         std::move(info));
}

} // namespace basalt::gfx
