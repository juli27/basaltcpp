#pragma once

#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>

#include <gsl/span>

namespace basalt::gfx {

class Context {
public:
  static auto create(DevicePtr, SwapChainPtr, Info) -> ContextPtr;

  [[nodiscard]] auto gfx_info() const noexcept -> const Info&;

  [[nodiscard]] auto create_resource_cache() const -> ResourceCachePtr;

  auto submit(gsl::span<const CommandList>) const -> void;

  // don't use. Use create() function instead
  Context(DevicePtr, SwapChainPtr, Info);

  // engine-private
  [[nodiscard]] auto device() const noexcept -> const DevicePtr&;
  [[nodiscard]] auto swap_chain() const noexcept -> const SwapChainPtr&;

private:
  DevicePtr mDevice;
  SwapChainPtr mSwapChain;
  Info mInfo;
};

} // namespace basalt::gfx
