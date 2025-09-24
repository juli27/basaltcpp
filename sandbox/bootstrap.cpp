#include <basalt/api/bootstrap.h>

#include <basalt/sandbox/sandbox.h>
#include <basalt/sandbox/settings.h>

#include <basalt/api/gfx/backend/adapter.h>
#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/shared/config.h>

#include <algorithm>
#include <string>
#include <utility>

using namespace std::literals;
using namespace basalt;

namespace {

// TODO: move to runtime
auto operator==(gfx::DisplayMode const& l, gfx::DisplayMode const& r) -> bool {
  return l.width == r.width && l.height == r.height &&
         l.refreshRate == r.refreshRate;
}

auto is_valid(gfx::DisplayMode const& mode, gfx::AdapterInfo const& adapterInfo)
  -> bool {
  auto const& displayModes = [&] {
    for (auto const& adapterMode : adapterInfo.exclusiveModes) {
      if (adapterMode.displayFormat == gfx::ImageFormat::B8G8R8X8) {
        return adapterMode.displayModes;
      }
    }

    return gfx::DisplayModes{};
  }();

  auto const it =
    std::find_if(displayModes.cbegin(), displayModes.cend(),
                 [&](gfx::DisplayMode const& m) { return m == mode; });

  return it != displayModes.cend();
}

} // namespace

auto basalt::bootstrap_app(Config& config) -> AppLaunchInfo {
  config.set_bool("runtime.debugUI.enabled"s, true);

  // TODO: verify compatibility with the current platform capabilities
  auto settings =
    Settings::from_file("settings.toml"s).value_or(Settings{"settings.toml"s});

  auto canvasInfo = CanvasCreateInfo{};
  canvasInfo.mode = settings.windowMode;
  canvasInfo.configureGfxContext = [=](gfx::AdapterInfos const& adapters) {
    auto const& adapterInfo = adapters[settings.adapter];

    // TODO: stop hardcoding formats and modes which might not be supported
    auto info = GfxContextCreateInfo{};
    info.adapter = settings.adapter;
    info.colorFormat = gfx::ImageFormat::B8G8R8X8;
    info.depthStencilFormat = gfx::ImageFormat::D24S8;
    info.sampleCount = settings.multiSampleCount;
    info.exclusiveDisplayMode = is_valid(settings.displayMode, adapterInfo)
                                  ? settings.displayMode
                                  : adapterInfo.sharedModeInfo.displayMode;

    return info;
  };

  auto const createView = [=](Engine& engine) {
    return SandboxView::create(engine, std::move(settings));
  };

  return AppLaunchInfo{"Basalt Sandbox"s, canvasInfo, createView};
}
