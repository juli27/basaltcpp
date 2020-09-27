#pragma once

#include <runtime/gfx/backend/device.h>

#include "d3d9_custom.h"

#include <runtime/gfx/backend/commands.h>
#include <runtime/gfx/backend/command_list.h>
#include <runtime/shared/handle_pool.h>

#include <wrl/client.h>

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
  D3D9Device() = delete;
  explicit D3D9Device(Microsoft::WRL::ComPtr<IDirect3DDevice9> device);
  D3D9Device(const D3D9Device&) = delete;
  D3D9Device(D3D9Device&&) = delete;
  ~D3D9Device() override = default;

  auto operator=(const D3D9Device&) -> D3D9Device& = delete;
  auto operator=(D3D9Device&&) -> D3D9Device& = delete;

  [[nodiscard]]
  auto device() const -> Microsoft::WRL::ComPtr<IDirect3DDevice9>;

  void before_reset();
  void after_reset();

  void begin_execution() const;
  void execute(const CommandList&);
  void end_execution() const;

  auto add_mesh(
    void* data, i32 numVertices, const VertexLayout& layout,
    PrimitiveType primitiveType
  ) -> MeshHandle override;
  void remove_mesh(MeshHandle meshHandle) override;
  auto add_texture(std::string_view filePath) -> TextureHandle override;
  void remove_texture(TextureHandle textureHandle) override;
  auto load_model(std::string_view filePath) -> ModelHandle override;
  void remove_model(ModelHandle) override;

  void init_dear_imgui() override;
  void shutdown_dear_imgui() override;
  void new_gui_frame() override;

private:
  using Texture = Microsoft::WRL::ComPtr<IDirect3DTexture9>;

  struct Model {
    std::vector<D3DMATERIAL9> materials {};
    std::vector<Texture> textures {};
    Microsoft::WRL::ComPtr<ID3DXMesh> mesh {};
  };

  Microsoft::WRL::ComPtr<IDirect3DDevice9> mDevice {};
  D3DCAPS9 mDeviceCaps {};
  HandlePool<D3D9Mesh, MeshHandle> mMeshes {};
  HandlePool<Texture, TextureHandle> mTextures {};
  HandlePool<Model, ModelHandle> mModels {};
  u8 mMaxLightsUsed {0};

  void execute(const CommandLegacy&);
  void execute(const CommandSetAmbientLight&) const;
  void execute(const CommandSetDirectionalLights&);
  void execute(const CommandSetTransform&) const;
  static void execute(const CommandRenderImGui&);
};

} // namespace basalt::gfx
