#include <basalt/gfx/backend/validating_context.h>

#include <basalt/gfx/backend/validating_device.h>

#include <utility>

namespace basalt::gfx {

auto ValidatingContext::wrap(ContextPtr context) -> ValidatingContextPtr {
  return std::make_shared<ValidatingContext>(std::move(context));
}

ValidatingContext::ValidatingContext(ContextPtr context)
  : mContext {std::move(context)}
  , mDebugDevice {ValidatingDevice::wrap(mContext->device())} {
}

auto ValidatingContext::surface_size() const noexcept -> Size2Du16 {
  return mContext->surface_size();
}

auto ValidatingContext::get_status() const noexcept -> ContextStatus {
  return mContext->get_status();
}

auto ValidatingContext::reset() -> void {
  mContext->reset();
}

auto ValidatingContext::reset(const ResetDesc& desc) -> void {
  mContext->reset(desc);
}

auto ValidatingContext::device() const noexcept -> DevicePtr {
  return mDebugDevice;
}

auto ValidatingContext::present() -> PresentResult {
  return mContext->present();
}

} // namespace basalt::gfx
