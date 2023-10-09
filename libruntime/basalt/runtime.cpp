#include <basalt/runtime.h>

#include <basalt/dear_imgui.h>

#include <basalt/gfx/debug.h>

#include <basalt/gfx/backend/device.h>
#include <basalt/gfx/backend/swap_chain.h>
#include <basalt/gfx/backend/types.h>

#include <basalt/api/client_app.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/context.h>
#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/shared/config.h>

#include <string>
#include <utility>

namespace basalt {

using namespace std::literals;

auto Runtime::dear_imgui() const -> DearImGuiPtr const& {
  return mDearImGui;
}

auto Runtime::update(UpdateContext const& ctx) -> void {
  auto composite = gfx::Composite{};
  auto const drawCtx = View::DrawContext{
    composite,
    mGfxContext->swap_chain()->get_info().backBufferSize,
  };
  auto updateCtx = View::UpdateContext{*this, drawCtx, ctx.deltaTime};

  mDearImGui->new_frame(updateCtx);
  root()->update(updateCtx);

  // The DearImGui view doesn't actually cause the UI to render during drawing
  // but is currently being done at execution of the ExtRenderDearImGui
  // command instead.
  mDearImGui->update(updateCtx);

  if (config().get_bool("runtime.debugUI.enabled"s)) {
    gfx::Debug::update(composite);
  }

  mGfxContext->submit(composite);
}

Runtime::Runtime(Config& config, gfx::ContextPtr gfxContext,
                 DearImGuiPtr dearImGui)
  : Engine{config, std::move(gfxContext)}
  , mDearImGui{std::move(dearImGui)} {
  ClientApp::bootstrap(*this);
}

} // namespace basalt
