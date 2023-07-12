#pragma once

#include <basalt/gfx/backend/context.h>

#include <basalt/gfx/backend/types.h>

namespace basalt::gfx {

class ValidatingContext final : public Context {
public:
  static auto wrap(ContextPtr) -> ValidatingContextPtr;

  // don't use directly
  explicit ValidatingContext(ContextPtr);

  [[nodiscard]] auto surface_size() const noexcept -> Size2Du16 override;

  [[nodiscard]] auto get_status() const noexcept -> ContextStatus override;

  auto reset() -> void override;

  auto reset(const ResetDesc&) -> void override;

  [[nodiscard]] auto device() const noexcept -> DevicePtr override;

  [[nodiscard]] auto present() -> PresentResult override;

private:
  ContextPtr mContext;
  ValidatingDevicePtr mDebugDevice;
};

} // namespace basalt::gfx
