#pragma once

#include <basalt/api/types.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/shared/size2d.h>
#include <basalt/api/shared/types.h>

#include <functional>
#include <string>

namespace basalt {

using ViewFactory = ViewPtr(Engine&);

struct CanvasCreateInfo {
  gfx::Adapter adapter;
  gfx::DisplayMode exclusiveFullscreenMode{};
  Size2Du16 size{Size2Du16::dont_care()};
  gfx::ImageFormat renderTargetFormat{gfx::ImageFormat::Unknown};
  gfx::ImageFormat depthStencilFormat{gfx::ImageFormat::Unknown};
  gfx::MultiSampleCount sampleCount{gfx::MultiSampleCount::One};
  bool isUserResizeable{true};
  WindowMode initialMode{WindowMode::Windowed};
};

using ConfigureCanvas = CanvasCreateInfo(gfx::AdapterList const&);

struct AppLaunchInfo {
  std::function<ViewFactory> createRootView;
  std::string appName{};
  gfx::BackendApi gfxBackendApi{gfx::BackendApi::Default};
  std::function<ConfigureCanvas> configureCanvas{};
};

auto bootstrap_app(Config&) -> AppLaunchInfo;

} // namespace basalt
