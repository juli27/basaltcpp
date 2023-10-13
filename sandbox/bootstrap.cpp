#include <basalt/api/bootstrap.h>

#include <basalt/sandbox/sandbox.h>

#include <basalt/api/shared/config.h>

#include <string>

using namespace std::literals;

auto basalt::bootstrap_app() -> BasaltApp {
  BasaltApp app{
    &SandboxView::create,
    Config{
      {"debug.scene_inspector.visible"s, false},
      {"runtime.debugUI.enabled"s, true},
    },
  };

  app.windowTitle = "Basalt Sandbox"s;

  return app;
}
