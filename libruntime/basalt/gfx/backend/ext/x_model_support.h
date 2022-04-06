#pragma once

// transitive include
#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <basalt/gfx/backend/ext/extension.h>
#include <basalt/gfx/backend/ext/types.h>

#include <basalt/api/shared/color.h>

#include <filesystem>
#include <vector>

namespace basalt::gfx::ext {

class XModelSupport
  : public DeviceExtensionT<DeviceExtensionId::XModelSupport> {
public:
  [[nodiscard]] virtual auto load(std::filesystem::path const&)
    -> XModelData = 0;

  virtual auto destroy(XMeshHandle) noexcept -> void = 0;
};

struct XModelData final {
  struct Material final {
    Color diffuse;
    Color ambient;
    Color emissive;
    Color specular;
    f32 specularPower{};
    std::filesystem::path textureFile;
  };

  std::vector<XMeshHandle> meshes;
  std::vector<Material> materials;
};

} // namespace basalt::gfx::ext
