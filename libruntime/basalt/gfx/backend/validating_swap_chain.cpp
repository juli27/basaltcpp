#include <basalt/gfx/backend/validating_swap_chain.h>

#include <basalt/gfx/backend/validating_device.h>

#include <utility>

namespace basalt::gfx {

auto ValidatingSwapChain::wrap(SwapChainPtr swapChain,
                               ValidatingDevicePtr device)
  -> ValidatingSwapChainPtr {
  return std::make_shared<ValidatingSwapChain>(std::move(swapChain),
                                               std::move(device));
}

auto ValidatingSwapChain::device() const noexcept -> DevicePtr {
  return mValidatingDevice;
}

auto ValidatingSwapChain::get_info() const noexcept -> Info {
  return mSwapChain->get_info();
}

auto ValidatingSwapChain::reset(ResetDesc const& desc) -> void {
  mSwapChain->reset(desc);
}

auto ValidatingSwapChain::present() -> PresentResult {
  return mSwapChain->present();
}

ValidatingSwapChain::ValidatingSwapChain(SwapChainPtr swapChain,
                                         ValidatingDevicePtr device)
  : mSwapChain{std::move(swapChain)}
  , mValidatingDevice{std::move(device)} {
}

} // namespace basalt::gfx
