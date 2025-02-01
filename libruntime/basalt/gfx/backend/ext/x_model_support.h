#pragma once

// transitive include
#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include "extension.h"
#include "types.h"

#include <filesystem>

namespace basalt::gfx::ext {

class XModelSupport
  : public DeviceExtensionT<DeviceExtensionId::XModelSupport> {
public:
  [[nodiscard]]
  virtual auto load(std::filesystem::path const&) -> XModelData = 0;

  virtual auto destroy(XMeshHandle) noexcept -> void = 0;
};

} // namespace basalt::gfx::ext
