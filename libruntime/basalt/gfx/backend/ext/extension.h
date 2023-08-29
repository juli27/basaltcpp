#pragma once

#include <basalt/gfx/backend/ext/types.h>

namespace basalt::gfx::ext {

class DeviceExtension {
public:
  DeviceExtension(const DeviceExtension&) = delete;
  DeviceExtension(DeviceExtension&&) = delete;

  virtual ~DeviceExtension() noexcept = default;

  auto operator=(const DeviceExtension&) -> DeviceExtension& = delete;
  auto operator=(DeviceExtension&&) -> DeviceExtension& = delete;

protected:
  DeviceExtension() noexcept = default;
};

template <DeviceExtensionId Id>
class DeviceExtensionT : public DeviceExtension {
public:
  static constexpr DeviceExtensionId ID = Id;
};

} // namespace basalt::gfx::ext
