#include "compositor.h"

#include "drawable.h"
#include "visual.h"
#include "backend/context.h"
#include "backend/device.h"
#include "backend/render_command.h"

#include <runtime/shared/asserts.h>

#include <vector>
#include <utility>

using std::shared_ptr;
using std::vector;

namespace basalt::gfx {

Compositor::Compositor(shared_ptr<Context> context)
  : mContext {std::move(context)}, mDrawTarget {mContext->surface_size()} {
  BASALT_ASSERT(mContext);
}

auto Compositor::draw_target() -> DrawTarget& {
  mDrawTarget = DrawTarget {mContext->surface_size()};
  return mDrawTarget;
}

void Compositor::compose() {
  const vector<Visual>& visuals = mDrawTarget.visuals();
  BASALT_ASSERT_MSG(
    visuals.size() <= 1, "only one visual supported at this time");

  auto& device = mContext->device();

  if (!visuals.empty()) {
    device.render(visuals.front().drawable().draw(mContext->surface_size()));

    mDrawTarget.clear();
  }
}

} // namespace basalt::gfx
