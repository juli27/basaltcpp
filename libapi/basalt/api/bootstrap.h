#pragma once

#include "types.h"

#include "gfx/types.h"
#include "gfx/backend/adapter.h"
#include "gfx/backend/types.h"

#include "shared/size2d.h"
#include "shared/types.h"

#include "base/types.h"

#include <functional>
#include <string>

namespace basalt {

using ViewFactory = ViewPtr(Engine&);

struct CanvasCreateInfo {
  u32 adapter{};
  gfx::DisplayMode exclusiveFullscreenMode{};
  Size2Du16 size{Size2Du16::dont_care()};
  gfx::ImageFormat renderTargetFormat{gfx::ImageFormat::Unknown};
  gfx::ImageFormat depthStencilFormat{gfx::ImageFormat::Unknown};
  gfx::MultiSampleCount sampleCount{gfx::MultiSampleCount::One};
  bool isUserResizeable{true};
  WindowMode initialMode{WindowMode::Windowed};
};

using ConfigureCanvas = CanvasCreateInfo(gfx::AdapterInfos const&);

struct AppLaunchInfo {
  std::function<ViewFactory> createRootView;
  std::string appName{};
  gfx::BackendApi gfxBackendApi{gfx::BackendApi::Default};
  std::function<ConfigureCanvas> configureCanvas{};
};

auto bootstrap_app(Config&) -> AppLaunchInfo;

} // namespace basalt
