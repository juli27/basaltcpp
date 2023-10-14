#pragma once

#include <basalt/api/types.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/shared/config.h>
#include <basalt/api/shared/size2d.h>
#include <basalt/api/shared/types.h>

#include <functional>
#include <string>

namespace basalt {

using ViewFactory = ViewPtr(Engine&);

using ConfigureGfxContext = gfx::ContextCreateInfo(gfx::AdapterList const&);

struct BasaltApp {
  std::function<ViewFactory> createRootView;
  Config config;
  Size2Du16 canvasSize{Size2Du16::dont_care()};
  bool isCanvasResizeable{true};
  WindowMode windowMode{WindowMode::Windowed};
  std::string windowTitle{};
  gfx::BackendApi gfxBackendApi{gfx::BackendApi::Default};
  std::function<ConfigureGfxContext> gfxContextConfig{};
};

auto bootstrap_app() -> BasaltApp;

} // namespace basalt
