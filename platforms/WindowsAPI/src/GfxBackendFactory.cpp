#include <basalt/gfx/backend/IRenderer.h>

// TODO: Add macros to exclude certain backends from a build

#include <basalt/common/Exceptions.h>
#include <basalt/gfx/backend/d3d9/Renderer.h>
#include <basalt/platform/PlatformWindowsAPI.h>

#include <basalt/Log.h>

namespace basalt::gfx::backend {


auto IRenderer::Create(RendererType type) -> IRenderer* {
  switch (type) {
  case RendererType::Default:
  case RendererType::Direct3D9:
    return d3d9::Renderer::Create(platform::winapi::GetWindowHandle());
  default:
    BS_FATAL("RendererType {} not supported", static_cast<i8>(type));
    throw ApiNotSupportedException("RendererType not supported");
    break;
  }
}

} // namespace basalt::gfx::backend
