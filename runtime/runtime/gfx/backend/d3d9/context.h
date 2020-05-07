#pragma once
#ifndef BASALT_WIN32_D3D9_GFX_CONTEXT_H
#define BASALT_WIN32_D3D9_GFX_CONTEXT_H

#include "runtime/gfx/backend/d3d9/d3d9_custom.h"
#include "runtime/platform/IGfxContext.h"

#include <wrl/client.h>

namespace basalt::platform {

struct D3D9GfxContext final : IGfxContext {
  D3D9GfxContext(
    Microsoft::WRL::ComPtr<IDirect3DDevice9> device
  , const D3DPRESENT_PARAMETERS& pp
  );
  D3D9GfxContext(const D3D9GfxContext&) = delete;
  D3D9GfxContext(D3D9GfxContext&&) = delete;
  ~D3D9GfxContext() = default;

  auto operator=(const D3D9GfxContext&) -> D3D9GfxContext& = delete;
  auto operator=(D3D9GfxContext&&) -> D3D9GfxContext& = delete;

  auto create_renderer() -> gfx::backend::IRenderer* override;
  void present() override;

private:
  Microsoft::WRL::ComPtr<IDirect3DDevice9> mDevice {};
  D3DPRESENT_PARAMETERS mPresentParams {};
};

} // basalt::platform

#endif // BASALT_WIN32_D3D9_GFX_CONTEXT_H
