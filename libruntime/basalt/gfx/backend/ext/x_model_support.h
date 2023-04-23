#pragma once

// transitive include
#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <basalt/gfx/backend/ext/extension.h>
#include <basalt/gfx/backend/ext/types.h>

#include <basalt/api/gfx/backend/ext/types.h>

#include <filesystem>

namespace basalt::gfx::ext {

class XModelSupport : public ExtensionT<ExtensionId::XModelSupport> {
public:
  [[nodiscard]] virtual auto load(const std::filesystem::path&)
    -> XModelData = 0;

  virtual auto destroy(XMesh) noexcept -> void = 0;
};

} // namespace basalt::gfx::ext
