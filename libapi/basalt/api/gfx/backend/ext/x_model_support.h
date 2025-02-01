#pragma once

#include "types.h"

#include "basalt/api/gfx/backend/types.h"

#include "basalt/api/shared/color.h"

#include <filesystem>
#include <vector>

namespace basalt::gfx::ext {

class XMeshCommandEncoder final {
public:
  static auto draw_x_mesh(CommandList&, XMeshHandle) -> void;
};

struct XModelData {
  struct Material {
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
