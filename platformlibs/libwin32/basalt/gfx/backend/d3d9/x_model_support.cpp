#include <basalt/gfx/backend/d3d9/x_model_support.h>

#include <basalt/gfx/backend/d3d9/conversions.h>

#include <basalt/gfx/backend/commands.h>

#include <stdexcept>
#include <string_view>
#include <utility>
#include <vector>

namespace basalt::gfx::ext {

using namespace std::literals;
using std::string_view;
using std::vector;
using std::filesystem::path;

using Microsoft::WRL::ComPtr;

namespace {

constexpr auto to_color(const D3DCOLORVALUE& color) -> Color {
  return Color::from_non_linear(color.r, color.g, color.b, color.a);
}

} // namespace

auto D3D9XModelSupport::create(DevicePtr device) -> D3D9XModelSupportPtr {
  return std::make_shared<D3D9XModelSupport>(std::move(device));
}

auto D3D9XModelSupport::execute(const CommandDrawXMesh& cmd) const -> void {
  const auto& mesh {mMeshes[cmd.xMeshId]};

  D3D9CHECK(mesh->DrawSubset(cmd.subset));
}

auto D3D9XModelSupport::load(const path& filepath) -> XModelData {
  XMeshPtr mesh;

  ComPtr<ID3DXBuffer> materialBuffer;
  DWORD numMaterials {};
  if (FAILED(D3DXLoadMeshFromXW(filepath.c_str(), D3DXMESH_MANAGED,
                                mDevice.Get(), nullptr, &materialBuffer,
                                nullptr, &numMaterials, &mesh))) {
    throw std::runtime_error {"loading mesh file failed"};
  }

  vector<XModelData::Material> materials {};

  materials.reserve(numMaterials);
  const auto* const d3dMaterials {
    static_cast<const D3DXMATERIAL*>(materialBuffer->GetBufferPointer())};

  for (DWORD i {0}; i < numMaterials; i++) {
    const auto& d3dMaterial {d3dMaterials[i].MatD3D};

    const Color diffuse {to_color(d3dMaterial.Diffuse)};
    auto& material {materials.emplace_back(XModelData::Material {
      diffuse,
      diffuse, // d3dx doesn't set the ambient color
    })};

    if (!d3dMaterials[i].pTextureFilename) {
      continue;
    }

    const string_view texFileName {d3dMaterials[i].pTextureFilename};
    if (texFileName.empty()) {
      continue;
    }

    path texPath {filepath.parent_path() / texFileName};
    material.textureFile = std::move(texPath);
  }

  const XMesh meshHandle {mMeshes.allocate(std::move(mesh))};
  return XModelData {meshHandle, std::move(materials)};
}

auto D3D9XModelSupport::destroy(const XMesh handle) noexcept -> void {
  mMeshes.deallocate(handle);
}

D3D9XModelSupport::D3D9XModelSupport(DevicePtr device)
  : mDevice {std::move(device)} {
}

} // namespace basalt::gfx::ext
