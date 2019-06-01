#include <basalt/gfx/backend/Factory.h>

#include <basalt/gfx/backend/IRenderer.h>
#include <basalt/gfx/backend/d3d9/Factory.h>
#include <basalt/platform/PlatformWindowsAPI.h>

namespace basalt {
namespace gfx {
namespace backend {

void Shutdown() {
  d3d9::Shutdown();
}

IRenderer* CreateRenderer() {
  return d3d9::CreateRenderer(platform::winapi::GetWindowHandle());
}

} // namespace backend
} // namespace gfx
} // namespace basalt
