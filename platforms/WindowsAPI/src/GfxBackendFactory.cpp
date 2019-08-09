#include <basalt/gfx/backend/IRenderer.h>

// TODO: Add macros to exclude certain backends from a build

#include <basalt/common/Exceptions.h>
#include <basalt/gfx/backend/d3d9/Renderer.h>
#include <basalt/platform/PlatformWindowsAPI.h>

#include <basalt/Log.h>

namespace basalt {
namespace gfx {
namespace backend {


IRenderer* IRenderer::Create(RendererType type) {
  switch (type) {
  case RendererType::DEFAULT:
  case RendererType::DIRECT3D9:
    return d3d9::Renderer::Create(platform::winapi::GetWindowHandle());
  default:
    BS_FATAL("RendererType {} not supported", static_cast<i8>(type));
    throw ApiNotSupportedException("RendererType not supported");
    break;
  }
}

} // namespace backend
} // namespace gfx
} // namespace basalt
