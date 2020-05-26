#pragma once
#ifndef BASALT_APP_D3D9_RENDERER_H
#define BASALT_APP_D3D9_RENDERER_H

#include "runtime/gfx/backend/IRenderer.h"

#include "d3d9_custom.h"

#include "runtime/gfx/backend/render_command.h"

#include "runtime/shared/HandlePool.h"

#include <wrl/client.h>

#include <vector>

namespace basalt::gfx::backend {

struct D3D9Mesh {
  Microsoft::WRL::ComPtr<IDirect3DVertexBuffer9> vertexBuffer {};
  DWORD fvf {0u};
  UINT vertexSize {0u};
  D3DPRIMITIVETYPE primType {D3DPT_POINTLIST};
  UINT primCount {0u};
};

struct D3D9Renderer final : IRenderer {
  D3D9Renderer() = delete;
  explicit D3D9Renderer(Microsoft::WRL::ComPtr<IDirect3DDevice9> device);
  D3D9Renderer(const D3D9Renderer&) = delete;
  D3D9Renderer(D3D9Renderer&&) = delete;
  ~D3D9Renderer();

  auto operator=(const D3D9Renderer&) -> D3D9Renderer& = delete;
  auto operator=(D3D9Renderer&&) -> D3D9Renderer& = delete;

  void before_reset();
  void after_reset();

  auto add_mesh(
    void* data, i32 numVertices, const VertexLayout& layout,
    PrimitiveType primitiveType
  ) -> MeshHandle override;
  void remove_mesh(MeshHandle meshHandle) override;
  auto add_texture(std::string_view filePath) -> TextureHandle override;
  void remove_texture(TextureHandle textureHandle) override;
  auto load_model(std::string_view filePath) -> ModelHandle override;
  void remove_model(ModelHandle) override;
  void set_clear_color(const Color& color) override;
  void render(const RenderCommandList&) override;

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
  D3DCOLOR mClearColor {D3DCOLOR_XRGB(0, 0, 0)};

  void render_command(const RenderCommand&);
};

} // namespace basalt::gfx::backend

#endif // !BASALT_APP_D3D9_RENDERER_H
