#include "D3D9GfxContext.h"

#include "runtime/gfx/backend/d3d9/D3D9Renderer.h"

#include "runtime/shared/Log.h"

namespace basalt::platform {

D3D9GfxContext::D3D9GfxContext(
  Microsoft::WRL::ComPtr<IDirect3DDevice9> device
, const D3DPRESENT_PARAMETERS& pp
) : mDevice {std::move(device)}, mPresentParams {pp} {
}

auto D3D9GfxContext::create_renderer() -> gfx::backend::IRenderer* {
  return new gfx::backend::D3D9Renderer(mDevice.Get(), mPresentParams);
}

void D3D9GfxContext::present() {
  mDevice->Present(nullptr, nullptr, nullptr, nullptr);
}

} // basalt::platform
