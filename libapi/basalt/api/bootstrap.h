#pragma once

#include "types.h"

#include "gfx/types.h"
#include "gfx/backend/adapter.h"
#include "gfx/backend/types.h"

#include "shared/size2d.h"
#include "shared/types.h"

#include "base/types.h"

#include <functional>
#include <optional>
#include <string>

namespace basalt {

struct GfxContextCreateInfo {
  u32 adapter{};
  gfx::ImageFormat colorFormat;
  std::optional<gfx::ImageFormat> depthStencilFormat;
  gfx::MultiSampleCount sampleCount;
  std::optional<gfx::DisplayMode> exclusiveDisplayMode;
};

using ConfigureGfxContextFn = GfxContextCreateInfo(gfx::AdapterInfos const&);

struct CanvasCreateInfo {
  Size2Du16 size;
  bool isUserResizeable{true};
  WindowMode mode{WindowMode::Windowed};
  gfx::BackendApi gfxBackendApi{gfx::BackendApi::Default};
  std::function<ConfigureGfxContextFn> configureGfxContext;
};

using ViewFactoryFn = ViewPtr(Engine&);

struct AppLaunchInfo {
  std::string appName;
  CanvasCreateInfo canvasCreateInfo;
  std::function<ViewFactoryFn> createRootView;
};

auto bootstrap_app(Config&) -> AppLaunchInfo;

} // namespace basalt
