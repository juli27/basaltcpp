#pragma once
#ifndef BASALT_WIN32_D3D9_GFX_CONTEXT_H
#define BASALT_WIN32_D3D9_GFX_CONTEXT_H

#include <basalt/gfx/backend/D3D9Header.h>
#include <basalt/platform/IGfxContext.h>

namespace basalt::platform {

struct D3D9GfxContext final : IGfxContext {
  explicit D3D9GfxContext(HWND window);
  D3D9GfxContext(const D3D9GfxContext&) = delete;
  D3D9GfxContext(D3D9GfxContext&&) = delete;
  ~D3D9GfxContext();

  auto operator=(const D3D9GfxContext&) -> D3D9GfxContext& = delete;
  auto operator=(D3D9GfxContext&&) -> D3D9GfxContext& = delete;

  auto create_renderer() -> gfx::backend::IRenderer* override;
  void present() override;

private:
  IDirect3D9* mD3D9Object = nullptr;
  IDirect3DDevice9* mDevice = nullptr;
  D3DPRESENT_PARAMETERS mPresentParams = {};
};

} // basalt::platform

#endif // BASALT_WIN32_D3D9_GFX_CONTEXT_H
