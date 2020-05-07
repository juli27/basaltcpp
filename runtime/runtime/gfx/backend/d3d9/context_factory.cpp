#include "runtime/gfx/backend/d3d9/context_factory.h"

#include "runtime/gfx/backend/d3d9/util.h"
#include "runtime/platform/Platform.h"
#include "runtime/shared/Log.h"

#include <utility>

namespace basalt::gfx::backend {

using Microsoft::WRL::ComPtr;

using std::unique_ptr;

D3D9ContextFactory::D3D9ContextFactory(ComPtr<IDirect3D9> factory)
  : mFactory {std::move(factory)} {
}

auto D3D9ContextFactory::create_context(const HWND window) const -> unique_ptr<D3D9GfxContext> {
  D3DPRESENT_PARAMETERS pp {
    0u, 0u, D3DFMT_UNKNOWN, 1u, // back buffer
    D3DMULTISAMPLE_NONE, 0u, // multi sampling
    D3DSWAPEFFECT_DISCARD, window, TRUE, // window
    TRUE, D3DFMT_D16, 0u, // depth stencil buffer + flags
    0u, D3DPRESENT_INTERVAL_ONE // refresh rate + VSync
  };

  const auto windowMode = platform::get_window_mode();

  // setup exclusive fullscreen
  if (windowMode == WindowMode::FullscreenExclusive) {
    D3DDISPLAYMODE displayMode {};
    D3D9CALL(
      mFactory->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode)
    );

    pp.BackBufferWidth = displayMode.Width;
    pp.BackBufferHeight = displayMode.Height;
    pp.BackBufferFormat = displayMode.Format;
    pp.Windowed = FALSE;
    pp.FullScreen_RefreshRateInHz = displayMode.RefreshRate;
  }

  ComPtr<IDirect3DDevice9> device {};
  D3D9CALL(
    mFactory->CreateDevice(
      D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window,
      D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp, device.GetAddressOf()
    )
  );

  return std::make_unique<D3D9GfxContext>(std::move(device), pp);
}

auto D3D9ContextFactory::create() -> std::optional<std::unique_ptr<
  D3D9ContextFactory>> {
  ComPtr<IDirect3D9> factory {};
  factory.Attach(Direct3DCreate9(D3D_SDK_VERSION));
  if (!factory) {
    BASALT_LOG_INFO("failed to create IDirect3D9 object");

    return {};
  }

  if (!D3DXCheckVersion(D3D_SDK_VERSION, D3DX_SDK_VERSION)) {
    BASALT_LOG_INFO("D3DX version missmatch");

    return {};
  }

  return std::make_unique<D3D9ContextFactory>(std::move(factory));
}

} // namespace basalt::gfx::backend
