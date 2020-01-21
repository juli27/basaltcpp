#pragma once
#ifndef BASALT_GFX_BACKEND_D3D9_RENDERER_H
#define BASALT_GFX_BACKEND_D3D9_RENDERER_H

#include "D3D9Header.h"

#include "runtime/gfx/backend/IRenderer.h"

#include "runtime/shared/HandlePool.h"

namespace basalt::gfx::backend {

struct D3D9Mesh {
  IDirect3DVertexBuffer9* vertexBuffer = nullptr;
  DWORD fvf = 0u;
  UINT vertexSize = 0u;
  D3DPRIMITIVETYPE primType = D3DPT_POINTLIST;
  UINT primCount = 0u;
};

struct D3D9Texture {
  IDirect3DTexture9* texture = nullptr;
};

struct D3D9Renderer final : IRenderer {
  D3D9Renderer() = delete;
  explicit D3D9Renderer(IDirect3DDevice9* device, const D3DPRESENT_PARAMETERS& pp);
  D3D9Renderer(const D3D9Renderer&) = delete;
  D3D9Renderer(D3D9Renderer&&) = delete;
  ~D3D9Renderer();

  auto operator=(const D3D9Renderer&) -> D3D9Renderer& = delete;
  auto operator=(D3D9Renderer&&) -> D3D9Renderer& = delete;

  void on_window_resize(const platform::WindowResizedEvent& event) override;
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
  void render_commands(const RenderCommandBuffer& commands);

  IDirect3DDevice9* mDevice = nullptr;
  D3DCAPS9 mDeviceCaps = {};
  D3DPRESENT_PARAMETERS mPresentParams;
  HandlePool<D3D9Mesh, MeshHandle> mMeshes;
  HandlePool<D3D9Texture, TextureHandle> mTextures;
  RenderCommandBuffer mCommandBuffer;
  D3DCOLOR mClearColor = D3DCOLOR_XRGB(0, 0, 0);
};

} // namespace basalt::gfx::backend

#endif // !BASALT_GFX_BACKEND_D3D9_RENDERER_H
