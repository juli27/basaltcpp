#pragma once

#include <basalt/gfx/backend/ext/x_model_support.h>

#include <basalt/gfx/backend/d3d9/d3d9_custom.h>
#include <basalt/gfx/backend/d3d9/types.h>

#include <basalt/gfx/backend/types.h>

#include <basalt/api/gfx/backend/ext/types.h>
#include <basalt/api/shared/handle_pool.h>

#include <wrl/client.h>

#include <filesystem>

namespace basalt::gfx::ext {

class D3D9XModelSupport final : public XModelSupport {
  using DevicePtr = Microsoft::WRL::ComPtr<IDirect3DDevice9>;

public:
  static auto create(DevicePtr) -> D3D9XModelSupportPtr;

  auto execute(const CommandDrawXMesh& cmd) const -> void;

  [[nodiscard]] auto load(const std::filesystem::path& filepath)
    -> XModelData override;

  auto destroy(XMesh handle) noexcept -> void override;

  explicit D3D9XModelSupport(DevicePtr);

private:
  using XMeshPtr = Microsoft::WRL::ComPtr<ID3DXMesh>;

  struct XMeshData {
    XMeshPtr mesh;
    DWORD attributeId {};
  };

  DevicePtr mDevice;
  HandlePool<XMeshData, XMesh> mMeshes;
};

} // namespace basalt::gfx::ext
