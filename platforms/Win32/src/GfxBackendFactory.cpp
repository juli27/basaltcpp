#include <basalt/gfx/backend/IRenderer.h>

// TODO: Add macros to exclude certain backends from a build

#include <basalt/common/Exceptions.h>
#include <basalt/gfx/backend/d3d9/Renderer.h>
#include "Win32Platform.h"

#include <basalt/Log.h>

namespace basalt::gfx::backend {

// TODO: IGfxContext per window which can query hardware support, create ISwapchains and create renderers
//       D3D9Context, DXGIContext, WGLContext
// a GfxContext gets created with the window according to the passed RendererType
// renderer are then created and assigned a swapchain.
// OR have renderer targets which the swapchain provides
// A window can be incompatible for certain APIs

auto IRenderer::Create(RendererType type) -> IRenderer* {
  switch (type) {
  case RendererType::Default:
  case RendererType::Direct3D9:
    return d3d9::Renderer::Create(platform::winapi::GetWindowHandle());
  default:
    BS_FATAL("RendererType {} not supported", static_cast<i8>(type));
    throw ApiNotSupportedException("RendererType not supported");
  }
}

} // namespace basalt::gfx::backend
