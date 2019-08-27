#include <basalt/platform/D3D9GfxContext.h>

#include <basalt/common/Exceptions.h>
#include <basalt/gfx/backend/d3d9/Renderer.h>
#include <basalt/gfx/backend/d3d9/Util.h>
#include <basalt/platform/Platform.h>
#include <basalt/platform/WindowTypes.h>

#include <basalt/Log.h>

namespace basalt::platform::winapi {

D3D9GfxContext::D3D9GfxContext(HWND window) {
  mD3D9Object = Direct3DCreate9(D3D_SDK_VERSION);
  if (!mD3D9Object) {
    BS_WARN("failed to create IDirect3D9 object");
    throw ApiNotSupportedException("Direct3D 9 not available");
  }

  if (!D3DXCheckVersion(D3D_SDK_VERSION, D3DX_SDK_VERSION)) {
    BS_WARN("D3DX version missmatch");
    throw ApiNotSupportedException("D3DX version missmatch");
  }

  D3DPRESENT_PARAMETERS pp{
    0u, 0u, D3DFMT_UNKNOWN, 1u, // back buffer
    D3DMULTISAMPLE_NONE, 0u, // multi sampling
    D3DSWAPEFFECT_DISCARD, window, TRUE, // window
    TRUE, D3DFMT_D16, 0u, // depth stencil buffer + flags
    0u, D3DPRESENT_INTERVAL_ONE // refresh rate + vsync
  };

  const auto& windowData = platform::get_window_data();

  // setup exclusive fullscreen
  if (windowData.mMode == WindowMode::FullscreenExclusive) {
    D3DDISPLAYMODE displayMode{};
    D3D9CALL(
      mD3D9Object->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode)
    );

    pp.BackBufferWidth = displayMode.Width;
    pp.BackBufferHeight = displayMode.Height;
    pp.BackBufferFormat = displayMode.Format;
    pp.Windowed = FALSE;
    pp.FullScreen_RefreshRateInHz = displayMode.RefreshRate;
  }

  D3D9CALL(
    mD3D9Object->CreateDevice(
      D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window,
      D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp, &mDevice
    )
  );

  mPresentParams = pp;
}

D3D9GfxContext::~D3D9GfxContext() {
  mDevice->Release();
  mD3D9Object->Release();
}

auto D3D9GfxContext::create_renderer() -> gfx::backend::IRenderer* {
  return new gfx::backend::d3d9::Renderer(mDevice, mPresentParams);
}

void D3D9GfxContext::present() {
  mDevice->Present(nullptr, nullptr, nullptr, nullptr);
}

} // basalt::platform::winapi