#include <basalt/api/gfx/context.h>

#include <basalt/api/gfx/resource_cache.h>
#include <basalt/gfx/backend/device.h>

#if BASALT_DEV_BUILD
#include <basalt/gfx/backend/validating_device.h>
#include <basalt/gfx/backend/validating_swap_chain.h>
#endif

#include <basalt/api/shared/asserts.h>

#include <memory>
#include <utility>

namespace basalt::gfx {

auto Context::create(DevicePtr device, SwapChainPtr swapChain, Info info)
  -> ContextPtr {
  return std::make_shared<Context>(std::move(device), std::move(swapChain),
                                   std::move(info));
}

auto Context::gfx_info() const noexcept -> Info const& {
  return mInfo;
}

auto Context::create_resource_cache() const -> ResourceCachePtr {
  return ResourceCache::create(mDevice);
}

auto Context::submit(gsl::span<CommandList const> const cmdLists) const
  -> void {
  mDevice->submit(cmdLists);
}

Context::Context(DevicePtr device, SwapChainPtr swapChain, Info info)
  : mDevice{std::move(device)}
  , mSwapChain{std::move(swapChain)}
  , mInfo{std::move(info)} {
  BASALT_ASSERT(mDevice);
  BASALT_ASSERT(mSwapChain);

#if BASALT_DEV_BUILD
  auto wrappedDevice = ValidatingDevice::wrap(std::move(mDevice));
  mDevice = wrappedDevice;
  mSwapChain =
    ValidatingSwapChain::wrap(std::move(mSwapChain), std::move(wrappedDevice));
#endif
}

auto Context::device() const noexcept -> DevicePtr const& {
  return mDevice;
}

auto Context::swap_chain() const noexcept -> SwapChainPtr const& {
  return mSwapChain;
}

} // namespace basalt::gfx
