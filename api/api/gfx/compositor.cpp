#include "compositor.h"

#include "draw_target.h"
#include "drawable.h"

#include "backend/composite.h"

namespace basalt::gfx {

auto Compositor::compose(ResourceCache& resourceCache,
                         const DrawTarget& drawTarget) -> Composite {
  Composite composite;

  for (const auto& drawable : drawTarget.drawables()) {
    composite.add_part(drawable->draw(resourceCache, drawTarget.size()));

    const auto clearColor = drawable->clear_color();
    if (clearColor) {
      composite.set_background(*clearColor);
    }
  }

  return composite;
}

} // namespace basalt::gfx
