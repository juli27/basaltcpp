#include "runtime/gfx/backend/d3d9/context.h"

#include "runtime/gfx/backend/d3d9/renderer.h"

#include "runtime/shared/Log.h"

using std::unique_ptr;

namespace basalt::gfx::backend {

D3D9GfxContext::D3D9GfxContext(
  Microsoft::WRL::ComPtr<IDirect3DDevice9> device
, const D3DPRESENT_PARAMETERS& pp
)
  : mDevice {std::move(device)}, mPresentParams {pp} {
}

auto D3D9GfxContext::create_renderer() -> unique_ptr<IRenderer> {
  return std::make_unique<D3D9Renderer>(mDevice, mPresentParams);
}

void D3D9GfxContext::present() {
  mDevice->Present(nullptr, nullptr, nullptr, nullptr);
}

} // basalt::gfx::backend
