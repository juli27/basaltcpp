#pragma once

#include <api/gfx/backend/device.h>

#include "d3d9_custom.h"

#include <api/gfx/backend/commands.h>
#include <api/gfx/backend/command_list.h>
#include <api/shared/handle_pool.h>

#include <wrl/client.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace basalt::gfx {

struct D3D9Mesh {
  Microsoft::WRL::ComPtr<IDirect3DVertexBuffer9> vertexBuffer {};
  DWORD fvf {0u};
  UINT vertexSize {0u};
  D3DPRIMITIVETYPE primType {D3DPT_POINTLIST};
  UINT primCount {0u};
};

struct D3D9Device final : Device {
  explicit D3D9Device(Microsoft::WRL::ComPtr<IDirect3DDevice9> device);

  [[nodiscard]] auto device() const -> Microsoft::WRL::ComPtr<IDirect3DDevice9>;

  void before_reset();
  void after_reset();

  void begin_execution() const;
  void execute(const CommandList&);
  void end_execution() const;

  auto add_mesh(void* data, i32 numVertices, const VertexLayout& layout,
                PrimitiveType primitiveType) -> MeshHandle override;
  void remove_mesh(MeshHandle meshHandle) override;
  auto add_texture(std::string_view filePath) -> TextureHandle override;
  void remove_texture(TextureHandle textureHandle) override;

  auto query_extension(std::string_view name)
    -> std::optional<ExtensionPtr> override;

private:
  using ExtensionMap = std::unordered_map<std::string, ExtensionPtr>;
  using Texture = Microsoft::WRL::ComPtr<IDirect3DTexture9>;

  Microsoft::WRL::ComPtr<IDirect3DDevice9> mDevice {};

  ExtensionMap mExtensions {};

  D3DCAPS9 mDeviceCaps {};

  HandlePool<D3D9Mesh, MeshHandle> mMeshes {};
  HandlePool<Texture, TextureHandle> mTextures {};

  u8 mMaxLightsUsed {0};

  void execute(const CommandLegacy&);
  void execute(const CommandSetAmbientLight&) const;
  void execute(const CommandSetDirectionalLights&);
  void execute(const CommandSetTransform&) const;
  void execute(const CommandSetRenderState&) const;
};

} // namespace basalt::gfx
