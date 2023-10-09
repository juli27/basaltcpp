#pragma once

#include <basalt/gfx/backend/ext/extension.h>

#include <basalt/api/gfx/backend/types.h>

#include <filesystem>

namespace basalt::gfx::ext {

class Texture3DSupport
  : public DeviceExtensionT<DeviceExtensionId::Texture3DSupport> {
public:
  [[nodiscard]] virtual auto load(std::filesystem::path const&) -> Texture = 0;
};

} // namespace basalt::gfx::ext
