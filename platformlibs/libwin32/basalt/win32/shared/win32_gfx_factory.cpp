#include <basalt/win32/shared/win32_gfx_factory.h>

#if BASALT_DEV_BUILD
#include <basalt/gfx/backend/validating_swap_chain.h>
#endif

namespace basalt::gfx {

auto Win32GfxFactory::create_device_and_swap_chain(
  const HWND window, const DeviceAndSwapChainDesc& desc) const
  -> DeviceAndSwapChain {
  DeviceAndSwapChain res {do_create_device_and_swap_chain(window, desc)};

#if BASALT_DEV_BUILD
  res.swapChain = ValidatingSwapChain::wrap(res.swapChain);
#endif

  return res;
}

} // namespace basalt::gfx
