#pragma once

#include "swap_chain.h"

#include "types.h"

#include <basalt/api/gfx/backend/types.h>

namespace basalt::gfx {

class ValidatingSwapChain final : public SwapChain {
public:
  static auto wrap(SwapChainPtr, ValidatingDevicePtr) -> ValidatingSwapChainPtr;

  [[nodiscard]]
  auto device() const noexcept -> DevicePtr override;

  [[nodiscard]]
  auto get_info() const noexcept -> Info override;

  auto reset(Info const&) -> void override;

  [[nodiscard]] auto present() -> PresentResult override;

  // don't use. use the wrap() function instead
  ValidatingSwapChain(SwapChainPtr, ValidatingDevicePtr);

private:
  SwapChainPtr mSwapChain;
  ValidatingDevicePtr mValidatingDevice;
};

} // namespace basalt::gfx
