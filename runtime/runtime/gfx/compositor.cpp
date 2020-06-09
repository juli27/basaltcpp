#include "compositor.h"

#include "visual.h"
#include "backend/IRenderer.h"

#include <runtime/shared/Asserts.h>

#include <utility>

using std::shared_ptr;
using std::vector;

namespace basalt::gfx {

using backend::IGfxContext;

Compositor::Compositor(shared_ptr<IGfxContext> context)
  : mContext {std::move(context)}, mDrawTarget {mContext->surface_size()} {
}

auto Compositor::draw_target() -> DrawTarget& {
  return mDrawTarget;
}

void Compositor::compose() {
  const vector<Visual>& visuals = mDrawTarget.visuals();
  BASALT_ASSERT_MSG(
    visuals.size() <= 1, "only one visual supported at this time");

  auto& renderer = mContext->renderer();

  if (!visuals.empty()) {
    renderer.render(visuals[0].drawable().draw(mContext->surface_size()));

    mDrawTarget.clear();
  }
}

} // namespace basalt::gfx
