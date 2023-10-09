#pragma once

#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>

#include <gsl/span>

namespace basalt::gfx {

class Context {
public:
  static auto create(DevicePtr, SwapChainPtr, Info) -> ContextPtr;

  [[nodiscard]] auto gfx_info() const noexcept -> Info const&;

  [[nodiscard]] auto create_resource_cache() const -> ResourceCachePtr;

  auto submit(gsl::span<CommandList const>) const -> void;

  // don't use. Use create() function instead
  Context(DevicePtr, SwapChainPtr, Info);

  // engine-private
  [[nodiscard]] auto device() const noexcept -> DevicePtr const&;
  [[nodiscard]] auto swap_chain() const noexcept -> SwapChainPtr const&;

private:
  DevicePtr mDevice;
  SwapChainPtr mSwapChain;
  Info mInfo;
};

} // namespace basalt::gfx
