#include <basalt/gfx/backend/d3d9/Factory.h>

#include <basalt/Log.h>
#include <basalt/common/Exceptions.h>
#include <basalt/gfx/backend/d3d9/D3D9Header.h>
#include <basalt/gfx/backend/d3d9/Renderer.h>
#include <basalt/gfx/backend/d3d9/Util.h>
#include <basalt/platform/Platform.h>

namespace basalt {
namespace gfx {
namespace backend {
namespace d3d9 {
namespace {

IDirect3D9* s_d3d9;


void EnsureInitialized() {
  if (s_d3d9) {
    return;
  }

  s_d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
  if (!s_d3d9) {
    BS_WARN("failed to create IDirect3D9 object");
    throw ApiNotSupportedException("Direct3D 9 not available");
  }

  if (!D3DXCheckVersion(D3D_SDK_VERSION, D3DX_SDK_VERSION)) {
    BS_WARN("D3DX version missmatch");
    throw ApiNotSupportedException("D3DX version missmatch");
  }
}

} // namespace


void Shutdown() {
  if (s_d3d9) {
    ULONG refCount = s_d3d9->Release();
    s_d3d9 = nullptr;
    if (refCount) {
      BS_WARN("IDirect3D9 is leaking");
    }
  }
}


IRenderer* CreateRenderer(HWND window) {
  EnsureInitialized();

  D3DPRESENT_PARAMETERS pp{
    0u, 0u, D3DFMT_UNKNOWN, 1u, // back buffer
    D3DMULTISAMPLE_NONE, 0u, // multi sampling
    D3DSWAPEFFECT_DISCARD, window, TRUE, // window
    TRUE, D3DFMT_D16, 0u, // depth stencil buffer + flags
    0u, D3DPRESENT_INTERVAL_ONE // refresh rate + vsync
  };

  const WindowDesc& windowDesc = platform::GetWindowDesc();

  // setup exclusive fullscreen
  if (windowDesc.mode == WindowMode::FULLSCREEN) {
    D3DDISPLAYMODE displayMode{};
    D3D9CALL(
      s_d3d9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode)
    );

    pp.BackBufferWidth = displayMode.Width;
    pp.BackBufferHeight = displayMode.Height;
    pp.BackBufferFormat = displayMode.Format;
    pp.Windowed = FALSE;
    pp.FullScreen_RefreshRateInHz = displayMode.RefreshRate;
  }

  IDirect3DDevice9* device = nullptr;
  D3D9CALL(
    s_d3d9->CreateDevice(
      D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window,
      D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp, &device
    )
  );

  Renderer* renderer = new Renderer(device);

  // the renderer took ownership
  device->Release();

  return renderer;
}

} // namespace d3d9
} // namespace backend
} // namespace gfx
} // namespace basalt
