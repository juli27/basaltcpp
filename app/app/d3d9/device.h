#pragma once

#include <api/gfx/backend/device.h>

#include "d3d9_custom.h"

#include <api/gfx/backend/commands.h>
#include <api/gfx/backend/command_list.h>
#include <api/shared/handle_pool.h>

#include <wrl/client.h>

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
  auto add_texture(std::string_view filePath) -> TextureHandle override;

  auto query_extension(ext::ExtensionId)
    -> std::optional<ext::ExtensionPtr> override;

private:
  using ExtensionMap = std::unordered_map<ext::ExtensionId, ext::ExtensionPtr>;
  using Texture = Microsoft::WRL::ComPtr<IDirect3DTexture9>;

  Microsoft::WRL::ComPtr<IDirect3DDevice9> mDevice;

  ExtensionMap mExtensions;

  D3DCAPS9 mDeviceCaps {};

  HandlePool<D3D9Mesh, MeshHandle> mMeshes;
  HandlePool<Texture, TextureHandle> mTextures;

  u8 mMaxLightsUsed {};

  // TODO: make these return bool / an error ?
  void execute(const CommandLegacy&);
  void execute(const CommandSetDirectionalLights&);
  void execute(const CommandSetTransform&) const;
  void execute(const CommandSetRenderState&) const;
};

} // namespace basalt::gfx
