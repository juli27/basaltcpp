#pragma once

#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/api/shared/color.h>

#include <gsl/span>

#include <filesystem>
#include <vector>

namespace basalt::gfx::ext {

class XModelData final {
public:
  struct Material final {
    Color diffuse {};
    Color ambient {};
    std::filesystem::path textureFile {};
  };

  explicit XModelData(XMesh, std::vector<Material>);

  [[nodiscard]] auto mesh() const noexcept -> XMesh;

  [[nodiscard]] auto materials() const noexcept -> gsl::span<const Material>;

private:
  std::vector<Material> mMaterials;
  XMesh mMesh;
};

} // namespace basalt::gfx::ext
