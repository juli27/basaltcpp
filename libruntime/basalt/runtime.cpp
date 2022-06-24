#include <basalt/runtime.h>

#include <basalt/dear_imgui.h>

#include <basalt/gfx/debug.h>

#include <basalt/gfx/backend/context.h>

#include <basalt/api/client_app.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/shared/config.h>

#include <string>
#include <utility>

namespace basalt {

using namespace std::literals;

auto Runtime::tick() -> void {
  mDearImGui->new_frame(*this, mGfxContext->surface_size());

  root()->tick(*this);
}

auto Runtime::render() -> void {
  const gfx::Composite composite {draw()};

  if (config().get_bool("runtime.debugUI.enabled"s)) {
    gfx::Debug::update(composite);
  }

  mGfxContext->submit(composite);
}

Runtime::Runtime(Config& config, gfx::Info gfxInfo, gfx::ContextPtr gfxContext,
                 DearImGuiPtr dearImGui)
  : Engine {config, std::move(gfxInfo), gfxContext->device()}
  , mGfxContext {std::move(gfxContext)}
  , mDearImGui {std::move(dearImGui)} {
  ClientApp::bootstrap(*this);
}

auto Runtime::draw() -> gfx::Composite {
  gfx::Composite composite;
  const View::DrawContext drawContext {
    composite,
    mGfxResourceCache,
    mGfxContext->surface_size(),
  };

  root()->draw(drawContext);

  // The DearImGui view doesn't actually cause the UI to render during drawing
  // but is currently being done at execution of the ExtRenderDearImGui
  // command instead.
  mDearImGui->draw(drawContext);

  return composite;
}

} // namespace basalt
