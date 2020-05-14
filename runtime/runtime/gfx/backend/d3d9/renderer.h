#pragma once
#ifndef BASALT_RUNTIME_GFX_BACKEND_D3D9_RENDERER_H
#define BASALT_RUNTIME_GFX_BACKEND_D3D9_RENDERER_H

#include "runtime/gfx/backend/d3d9/d3d9_custom.h"

#include "runtime/gfx/backend/IRenderer.h"
#include "runtime/gfx/backend/RenderCommand.h"

#include "runtime/shared/HandlePool.h"

#include <wrl/client.h>

namespace basalt::gfx::backend {

struct D3D9Mesh {
  Microsoft::WRL::ComPtr<IDirect3DVertexBuffer9> vertexBuffer {};
  DWORD fvf = 0u;
  UINT vertexSize = 0u;
  D3DPRIMITIVETYPE primType = D3DPT_POINTLIST;
  UINT primCount = 0u;
};

struct D3D9Renderer final : IRenderer {
  D3D9Renderer() = delete;
  D3D9Renderer(
    Microsoft::WRL::ComPtr<IDirect3DDevice9> device
  , const D3DPRESENT_PARAMETERS& pp
  );
  D3D9Renderer(const D3D9Renderer&) = delete;
  D3D9Renderer(D3D9Renderer&&) = delete;
  ~D3D9Renderer();

  auto operator=(const D3D9Renderer&) -> D3D9Renderer& = delete;
  auto operator=(D3D9Renderer&&) -> D3D9Renderer& = delete;

  void on_window_resize(Size2Du16 size) override;
  auto add_mesh(
    void* data, i32 numVertices, const VertexLayout& layout,
    PrimitiveType primitiveType
  ) -> MeshHandle override;
  void remove_mesh(MeshHandle meshHandle) override;
  auto add_texture(std::string_view filePath) -> TextureHandle override;
  void remove_texture(TextureHandle textureHandle) override;
  void submit(const RenderCommand& command) override;
  void set_view_proj(
    const math::Mat4f32& view, const math::Mat4f32& projection
  ) override;
  void set_lights(const LightSetup& lights) override;
  void set_clear_color(Color color) override;
  void render() override;
  auto name() -> std::string_view override;

  void new_gui_frame() override;

private:
  void render_commands(const RenderCommandList& commands);

  Microsoft::WRL::ComPtr<IDirect3DDevice9> mDevice {};
  D3DCAPS9 mDeviceCaps = {};
  D3DPRESENT_PARAMETERS mPresentParams;
  HandlePool<D3D9Mesh, MeshHandle> mMeshes;
  HandlePool<IDirect3DTexture9*, TextureHandle> mTextures;
  RenderCommandList mCommandBuffer;
  D3DCOLOR mClearColor = D3DCOLOR_XRGB(0, 0, 0);
};

} // namespace basalt::gfx::backend

#endif // !BASALT_RUNTIME_GFX_BACKEND_D3D9_RENDERER_H
