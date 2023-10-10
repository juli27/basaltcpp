#include <basalt/api/gfx/context.h>

#include <basalt/api/gfx/resource_cache.h>
#include <basalt/gfx/backend/device.h>

#if BASALT_DEV_BUILD
#include <basalt/gfx/backend/validating_device.h>
#include <basalt/gfx/backend/validating_swap_chain.h>
#endif

#include <basalt/api/shared/asserts.h>

#include <memory>
#include <optional>
#include <utility>

namespace basalt::gfx {

using gsl::span;
using std::nullopt;
using std::optional;

auto Context::create(DevicePtr device, ext::DeviceExtensions deviceExtensions,
                     SwapChainPtr swapChain, Info info) -> ContextPtr {
  return std::make_shared<Context>(std::move(device),
                                   std::move(deviceExtensions),
                                   std::move(swapChain), std::move(info));
}

auto Context::gfx_info() const noexcept -> Info const& {
  return mInfo;
}

auto Context::create_resource_cache() -> ResourceCachePtr {
  return ResourceCache::create(shared_from_this());
}

auto Context::submit(span<CommandList const> const cmdLists) const -> void {
  mDevice->submit(cmdLists);
}

Context::Context(DevicePtr device, ext::DeviceExtensions deviceExtensions,
                 SwapChainPtr swapChain, Info info)
  : mDevice{std::move(device)}
  , mDeviceExtensions{std::move(deviceExtensions)}
  , mSwapChain{std::move(swapChain)}
  , mInfo{std::move(info)} {
  BASALT_ASSERT(mDevice);
  BASALT_ASSERT(mSwapChain);

#if BASALT_DEV_BUILD
  auto wrappedDevice = ValidatingDevice::wrap(std::move(mDevice));
  mDevice = wrappedDevice;
  wrappedDevice->wrap_extensions(mDeviceExtensions);
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

auto Context::query_device_extension(ext::DeviceExtensionId const id)
  -> optional<ext::DeviceExtensionPtr> {
  if (auto const entry = mDeviceExtensions.find(id);
      entry != mDeviceExtensions.end()) {
    return entry->second;
  }

  return nullopt;
}

} // namespace basalt::gfx
