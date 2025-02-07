#include <basalt/api/bootstrap.h>

#include <basalt/sandbox/sandbox.h>

#include <basalt/api/gfx/backend/adapter.h>
#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/shared/config.h>

#include <string>

using namespace std::literals;

auto basalt::bootstrap_app(Config& config) -> AppLaunchInfo {
  config.set_bool("runtime.debugUI.enabled"s, true);

  AppLaunchInfo info{&SandboxView::create, "Basalt Sandbox"s};
  info.configureCanvas = [](gfx::AdapterInfos const&) {
    auto canvasInfo = CanvasCreateInfo{};
    canvasInfo.depthStencilFormat = gfx::ImageFormat::D24S8;

    return canvasInfo;
  };

  return info;
}
