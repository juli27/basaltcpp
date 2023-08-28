#include <basalt/api/gfx/context.h>

#include <basalt/api/gfx/resource_cache.h>
#include <basalt/gfx/backend/device.h>

#include <basalt/api/shared/asserts.h>

#include <memory>
#include <utility>

namespace basalt::gfx {

auto Context::create(DevicePtr device, SwapChainPtr swapChain, Info info)
  -> ContextPtr {
  return std::make_shared<Context>(std::move(device), std::move(swapChain),
                                   std::move(info));
}

auto Context::gfx_info() const noexcept -> const Info& {
  return mInfo;
}

auto Context::create_resource_cache() const -> ResourceCachePtr {
  return ResourceCache::create(mDevice);
}

auto Context::submit(const gsl::span<CommandList> cmdLists) const -> void {
  mDevice->submit(cmdLists);
}

Context::Context(DevicePtr device, SwapChainPtr swapChain, Info info)
  : mDevice {std::move(device)}
  , mSwapChain {std::move(swapChain)}
  , mInfo {std::move(info)} {
  BASALT_ASSERT(mDevice);
  BASALT_ASSERT(mSwapChain);
}

auto Context::device() const noexcept -> const DevicePtr& {
  return mDevice;
}

auto Context::swap_chain() const noexcept -> const SwapChainPtr& {
  return mSwapChain;
}

} // namespace basalt::gfx