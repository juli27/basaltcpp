#include "compositor.h"

#include <api/gfx/draw_target.h>
#include <api/gfx/drawable.h>

#include <api/gfx/backend/composite.h>

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
