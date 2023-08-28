#pragma once

#include <basalt/gfx/backend/ext/x_model_support.h>

#include <basalt/gfx/backend/d3d9/d3d9_custom.h>
#include <basalt/gfx/backend/d3d9/types.h>

#include <basalt/gfx/backend/types.h>

#include <basalt/api/gfx/backend/ext/types.h>
#include <basalt/api/shared/handle_pool.h>

#include <filesystem>

namespace basalt::gfx::ext {

class D3D9XModelSupport final : public XModelSupport {
public:
  static auto create(IDirect3DDevice9Ptr) -> D3D9XModelSupportPtr;

  auto execute(const CommandDrawXMesh& cmd) const -> void;

  [[nodiscard]] auto load(const std::filesystem::path& filepath)
    -> XModelData override;

  auto destroy(XMesh handle) noexcept -> void override;

  explicit D3D9XModelSupport(IDirect3DDevice9Ptr);

private:
  struct XMeshData {
    ID3DXMeshPtr mesh;
    DWORD attributeId {};
  };

  IDirect3DDevice9Ptr mDevice;
  HandlePool<XMeshData, XMesh> mMeshes;
};

} // namespace basalt::gfx::ext
