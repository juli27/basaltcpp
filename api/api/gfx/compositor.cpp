#include "compositor.h"

#include "draw_target.h"
#include "drawable.h"

#include "backend/composite.h"

namespace basalt::gfx {

auto Compositor::compose(ResourceCache& resourceCache, const DrawTarget& drawTarget)
  -> Composite {
  const auto& drawables = drawTarget.drawables();

  if (drawables.empty()) {
    return Composite {Colors::BLACK};
  }

  Composite composite {
    drawables.front()->clear_color().value_or(Colors::BLACK)};

  for (const auto& drawable : drawables) {
    composite.add_part(drawable->draw(resourceCache, drawTarget.size()));
  }

  return composite;
}

} // namespace basalt::gfx
