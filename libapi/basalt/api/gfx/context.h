#pragma once

#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>
#include <basalt/api/gfx/backend/ext/types.h>

#include <gsl/span>

#include <memory>
#include <optional>

namespace basalt::gfx {

struct ContextCreateInfo {
  [[nodiscard]]
  static auto create_default(AdapterList const&) -> ContextCreateInfo;

  Adapter adapter;
  DisplayMode exclusiveDisplayMode;
  ImageFormat renderTargetFormat;
  ImageFormat depthStencilFormat;
  MultiSampleCount sampleCount;
};

class Context final : public std::enable_shared_from_this<Context> {
public:
  static auto create(DevicePtr, ext::DeviceExtensions, SwapChainPtr, Info)
    -> ContextPtr;

  [[nodiscard]]
  auto gfx_info() const noexcept -> Info const&;

  [[nodiscard]]
  auto create_resource_cache() -> ResourceCachePtr;

  auto submit(gsl::span<CommandList const>) const -> void;

  // don't use. Use create() function instead
  Context(DevicePtr, ext::DeviceExtensions, SwapChainPtr, Info);

  // engine-private
  [[nodiscard]]
  auto device() const noexcept -> DevicePtr const&;
  [[nodiscard]]
  auto swap_chain() const noexcept -> SwapChainPtr const&;

  template <typename T>
  [[nodiscard]]
  auto query_device_extension() -> std::optional<std::shared_ptr<T>> {
    static_assert(std::is_base_of_v<ext::DeviceExtension, T>);

    if (auto const maybeExt = query_device_extension(T::ID)) {
      return std::static_pointer_cast<T>(*maybeExt);
    }

    return std::nullopt;
  }

private:
  DevicePtr mDevice;
  ext::DeviceExtensions mDeviceExtensions;
  SwapChainPtr mSwapChain;
  Info mInfo;

  [[nodiscard]] auto query_device_extension(ext::DeviceExtensionId)
    -> std::optional<ext::DeviceExtensionPtr>;
};

} // namespace basalt::gfx
