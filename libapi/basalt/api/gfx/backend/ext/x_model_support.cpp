#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <utility>

namespace basalt::gfx::ext {

using std::vector;

using gsl::span;

XModelData::XModelData(XMesh mesh, vector<Material> materials)
  : mMaterials {std::move(materials)}, mMesh {mesh} {
}

auto XModelData::mesh() const noexcept -> XMesh {
  return mMesh;
}

auto XModelData::materials() const noexcept -> span<const Material> {
  return span {mMaterials};
}

} // namespace basalt::gfx::ext
