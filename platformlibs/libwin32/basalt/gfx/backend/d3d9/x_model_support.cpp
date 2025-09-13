#include "x_model_support.h"

#include "conversions.h"
#include "d3d9_error.h"

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

auto D3D9XModelSupport::create(IDirect3DDevice9Ptr device)
  -> D3D9XModelSupportPtr {
  return std::make_shared<D3D9XModelSupport>(std::move(device));
}

auto D3D9XModelSupport::execute(CommandDrawXMesh const& cmd) const -> void {
  auto const& meshData = mMeshes[cmd.xMeshId];

  D3D9CHECK(meshData.mesh->DrawSubset(meshData.attributeId));
}

auto D3D9XModelSupport::load(path const& filepath) -> XModelData {
  auto mesh = ID3DXMeshPtr{};

  auto materialBuffer = ID3DXBufferPtr{};
  auto numMaterials = DWORD{0};
  if (FAILED(D3DXLoadMeshFromXW(filepath.c_str(), D3DXMESH_MANAGED,
                                mDevice.Get(), nullptr, &materialBuffer,
                                nullptr, &numMaterials, &mesh))) {
    throw std::runtime_error{"loading mesh file failed"};
  }

  BASALT_ASSERT(materialBuffer->GetBufferSize() / sizeof(D3DXMATERIAL) >=
                numMaterials);

  auto const d3dxMaterials = span<D3DXMATERIAL const>{
    static_cast<D3DXMATERIAL const*>(materialBuffer->GetBufferPointer()),
    numMaterials};
  auto meshes = vector<XMeshHandle>{};
  meshes.reserve(d3dxMaterials.size());
  auto materials = vector<XModelData::Material>{};
  materials.reserve(d3dxMaterials.size());
  auto attributeId = DWORD{0};
  for (auto const& [d3dMaterial, texFileName] : d3dxMaterials) {
    auto const meshHandle = mMeshes.emplace(XMeshData{mesh, attributeId++});
    meshes.push_back(meshHandle);

    auto const textureFileName = texFileName ? string_view{texFileName} : ""sv;
    auto texturePath = !textureFileName.empty()
                         ? filepath.parent_path() / textureFileName
                         : path{""};

    // d3dx meshes only have a face color instead of diffuse and ambient color,
    // so we will use it for both
    auto const faceColor = to_color(d3dMaterial.Diffuse);
    materials.emplace_back(XModelData::Material{
      faceColor,
      faceColor,
      to_color(d3dMaterial.Emissive),
      to_color(d3dMaterial.Specular),
      d3dMaterial.Power,
      std::move(texturePath),
    });
  }

  return XModelData{std::move(meshes), std::move(materials)};
}

auto D3D9XModelSupport::destroy(XMeshHandle const handle) noexcept -> void {
  mMeshes.destroy(handle);
}

D3D9XModelSupport::D3D9XModelSupport(IDirect3DDevice9Ptr device)
  : mDevice{std::move(device)} {
}

} // namespace basalt::gfx::ext
