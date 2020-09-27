#include "compositor.h"

#include "draw_target.h"
#include "drawable.h"

#include "backend/composite.h"
#include "backend/command_list.h"

#include <vector>

using std::vector;

namespace basalt::gfx {

auto Compositor::compose(
  Device& device, const DrawTarget& drawTarget) -> Composite {
  const vector<DrawablePtr>& drawables = drawTarget.drawables();
  if (drawables.empty()) {
    return Composite {};
  }

  Composite composite {drawables.front()->clear_color()};

  for (const auto& drawable : drawables) {
    composite.add_part(drawable->draw(device, drawTarget.size()));
  }

  return composite;
}

} // namespace basalt::gfx
