#include "runtime.h"

#include "dear_imgui.h"

#include <basalt/api/view.h>

#include "gfx/backend/swap_chain.h"
#include "gfx/backend/types.h"

#include <basalt/api/gfx/context.h>
#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/shared/config.h>

#include <utility>

namespace basalt {

auto Runtime::dear_imgui() const -> DearImGuiPtr const& {
  return mDearImGui;
}

auto Runtime::is_dirty() const noexcept -> bool {
  return mIsDirty;
}

auto Runtime::set_dirty(bool const isDirty) noexcept -> void {
  mIsDirty = isDirty;
}

auto Runtime::update(UpdateContext const& ctx) -> void {
  auto composite = gfx::Composite{};
  auto const drawCtx = View::DrawContext{
    composite,
    mGfxContext->swap_chain()->get_info().size(),
  };
  auto updateCtx = View::UpdateContext{*this, drawCtx, ctx.deltaTime};

  mDearImGui->new_frame(updateCtx);
  root()->update(updateCtx);

  // The DearImGui view doesn't actually cause the UI to render during drawing
  // but is currently being done at execution of the ExtRenderDearImGui
  // command instead.
  mDearImGui->update(updateCtx);

  mGfxContext->submit(composite);
}

Runtime::Runtime(Config config, gfx::ContextPtr gfxContext)
  : Engine{std::move(config), std::move(gfxContext)}
  , mDearImGui{DearImGui::create(*mGfxContext)} {
}

} // namespace basalt
