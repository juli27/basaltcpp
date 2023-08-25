#include <basalt/runtime.h>

#include <basalt/dear_imgui.h>

#include <basalt/gfx/debug.h>

#include <basalt/gfx/backend/device.h>
#include <basalt/gfx/backend/swap_chain.h>

#include <basalt/api/client_app.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/shared/config.h>

#include <string>
#include <utility>

namespace basalt {

using namespace std::literals;

auto Runtime::dear_imgui() const -> const DearImGuiPtr& {
  return mDearImGui;
}

auto Runtime::update(const UpdateContext& ctx) -> void {
  gfx::Composite composite;
  const View::DrawContext drawCtx {
    composite,
    mSwapChain->get_info().backBufferSize,
  };
  View::UpdateContext updateCtx {*this, drawCtx, ctx.deltaTime};

  mDearImGui->new_frame(updateCtx);
  root()->update(updateCtx);

  // The DearImGui view doesn't actually cause the UI to render during drawing
  // but is currently being done at execution of the ExtRenderDearImGui
  // command instead.
  mDearImGui->update(updateCtx);

  if (config().get_bool("runtime.debugUI.enabled"s)) {
    gfx::Debug::update(composite);
  }

  mSwapChain->device()->submit(composite);
}

Runtime::Runtime(Config& config, gfx::Info gfxInfo, gfx::SwapChainPtr swapChain,
                 DearImGuiPtr dearImGui)
  : Engine {config, std::move(gfxInfo), swapChain->device()}
  , mSwapChain {std::move(swapChain)}
  , mDearImGui {std::move(dearImGui)} {
  ClientApp::bootstrap(*this);
}

} // namespace basalt
