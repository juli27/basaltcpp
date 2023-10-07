#pragma once

// transitive include
#include <basalt/api/gfx/backend/ext/effect.h>

#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/gfx/backend/ext/extension.h>
#include <basalt/gfx/backend/ext/types.h>

#include <filesystem>

namespace basalt::gfx::ext {

class Effects : public DeviceExtensionT<DeviceExtensionId::Effects> {
public:
  [[nodiscard]] virtual auto compile(std::filesystem::path const&)
    -> CompileResult = 0;

  virtual auto destroy(EffectId) noexcept -> void = 0;
  [[nodiscard]] virtual auto get(EffectId) -> Effect& = 0;
};

} // namespace basalt::gfx::ext
