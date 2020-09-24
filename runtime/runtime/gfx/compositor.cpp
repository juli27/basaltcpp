#include "compositor.h"

#include "draw_target.h"
#include "drawable.h"
#include "visual.h"
#include "backend/context.h"
#include "backend/device.h"
#include "backend/command_list.h"

#include "runtime/shared/asserts.h"
#include "runtime/shared/color.h"

#include <vector>

using std::vector;

namespace basalt::gfx {

void Compositor::compose(Context& context, const DrawTarget& drawTarget) {
  const vector<Visual>& visuals = drawTarget.visuals();
  BASALT_ASSERT_MSG(
    visuals.size() <= 1, "only one visual supported at this time");

  auto& device = context.device();

  if (!visuals.empty()) {
    auto& drawable = visuals.front().drawable();
    context.clear(drawable.clear_color());

    device.render(drawable.draw(device, drawTarget.size()));
  }
}

} // namespace basalt::gfx
