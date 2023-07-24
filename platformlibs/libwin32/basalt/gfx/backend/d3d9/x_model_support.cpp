#include <basalt/gfx/backend/d3d9/x_model_support.h>

#include <basalt/gfx/backend/d3d9/conversions.h>

#include <basalt/gfx/backend/commands.h>

#include <gsl/span>

#include <stdexcept>
#include <string_view>
#include <utility>
#include <vector>

namespace basalt::gfx::ext {

using namespace std::literals;
using std::string_view;
using std::vector;
using std::filesystem::path;

using gsl::span;

using Microsoft::WRL::ComPtr;

auto D3D9XModelSupport::create(DevicePtr device) -> D3D9XModelSupportPtr {
  return std::make_shared<D3D9XModelSupport>(std::move(device));
}

auto D3D9XModelSupport::execute(const CommandDrawXMesh& cmd) const -> void {
  const auto& meshData {mMeshes[cmd.xMeshId]};

  D3D9CHECK(meshData.mesh->DrawSubset(meshData.attributeId));
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

  const span<const D3DXMATERIAL> d3dxMaterials {
    static_cast<const D3DXMATERIAL*>(materialBuffer->GetBufferPointer()),
    numMaterials};
  vector<XMesh> meshes;
  meshes.reserve(d3dxMaterials.size());
  vector<XModelData::Material> materials;
  materials.reserve(d3dxMaterials.size());
  DWORD attributeId {0};
  for (const auto& [d3dMaterial, texFileName] : d3dxMaterials) {
    const XMesh meshHandle {mMeshes.allocate(XMeshData {mesh, attributeId++})};
    meshes.push_back(meshHandle);

    const string_view textureFileName {texFileName ? texFileName : ""};
    path texturePath {
      !textureFileName.empty() ? filepath.parent_path() / textureFileName : ""};

    // d3dx meshes only have a face color instead of diffuse and ambient color,
    // so we will use it for both
    const Color faceColor {to_color(d3dMaterial.Diffuse)};
    materials.emplace_back(XModelData::Material {
      faceColor,
      faceColor,
      to_color(d3dMaterial.Emissive),
      to_color(d3dMaterial.Specular),
      d3dMaterial.Power,
      std::move(texturePath),
    });
  }

  return XModelData {std::move(meshes), std::move(materials)};
}

auto D3D9XModelSupport::destroy(const XMesh handle) noexcept -> void {
  mMeshes.deallocate(handle);
}

D3D9XModelSupport::D3D9XModelSupport(DevicePtr device)
  : mDevice {std::move(device)} {
}

} // namespace basalt::gfx::ext
