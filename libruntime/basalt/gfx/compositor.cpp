#include <basalt/gfx/compositor.h>

#include <basalt/api/gfx/drawable.h>
#include <basalt/api/gfx/surface.h>

#include <algorithm>

namespace basalt::gfx {

auto Compositor::compose(ResourceCache& resourceCache,
                         const Surface& drawTarget) -> Composite {
  Composite composite;

  const Size2Du16 viewport = drawTarget.size();

  const auto& drawables = drawTarget.drawables();
  std::for_each(
    drawables.begin(), drawables.end(), [&](const DrawablePtr& drawable) {
      composite.emplace_back(drawable->draw(resourceCache, viewport));
    });

  return composite;
}

} // namespace basalt::gfx
