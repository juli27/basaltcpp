#include <basalt/api/bootstrap.h>

#include <basalt/sandbox/sandbox.h>

#include <basalt/api/gfx/backend/adapter.h>
#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/shared/config.h>

#include <string>

using namespace std::literals;

auto basalt::bootstrap_app(Config& config) -> AppLaunchInfo {
  config.set_bool("runtime.debugUI.enabled"s, true);

  auto canvasInfo = CanvasCreateInfo{};
  canvasInfo.configureGfxContext = [](gfx::AdapterInfos const& adapters) {
    // TODO: stop hardcoding formats and modes which might not be supported
    auto info = GfxContextCreateInfo{};
    info.colorFormat = gfx::ImageFormat::B8G8R8X8;
    info.depthStencilFormat = gfx::ImageFormat::D24S8;
    info.exclusiveDisplayMode = adapters[0].sharedModeInfo.displayMode;

    return info;
  };

  return AppLaunchInfo{"Basalt Sandbox"s, canvasInfo, &SandboxView::create};
}
