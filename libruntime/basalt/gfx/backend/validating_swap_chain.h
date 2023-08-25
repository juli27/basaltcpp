#pragma once

#include <basalt/gfx/backend/swap_chain.h>

#include <basalt/gfx/backend/types.h>

namespace basalt::gfx {

class ValidatingSwapChain final : public SwapChain {
public:
  static auto wrap(SwapChainPtr) -> ValidatingSwapChainPtr;

  [[nodiscard]] auto device() const noexcept -> DevicePtr override;
  [[nodiscard]] auto get_info() const noexcept -> Info override;

  auto reset(const ResetDesc&) -> void override;

  [[nodiscard]] auto present() -> PresentResult override;

  // don't use. use the wrap() function instead
  explicit ValidatingSwapChain(SwapChainPtr);

private:
  SwapChainPtr mSwapChain;
  ValidatingDevicePtr mValidatingDevice;
};

} // namespace basalt::gfx
