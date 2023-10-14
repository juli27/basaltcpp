#include <basalt/api/bootstrap.h>

#include <basalt/sandbox/sandbox.h>

#include <basalt/api/gfx/context.h>
#include <basalt/api/gfx/types.h>

#include <basalt/api/shared/config.h>

#include <string>

using namespace std::literals;

namespace gfx = basalt::gfx;

namespace {

auto configure_gfx_context(gfx::AdapterList const& adapters)
  -> gfx::ContextCreateInfo {
  auto contextConfig = gfx::ContextCreateInfo::create_default(adapters);
  contextConfig.depthStencilFormat = gfx::ImageFormat::D24S8;

  return contextConfig;
}

} // namespace

auto basalt::bootstrap_app() -> BasaltApp {
  BasaltApp app{
    &SandboxView::create,
    Config{
      {"debug.scene_inspector.visible"s, false},
      {"runtime.debugUI.enabled"s, true},
    },
  };

  app.windowTitle = "Basalt Sandbox"s;
  app.gfxContextConfig = &configure_gfx_context;

  return app;
}
