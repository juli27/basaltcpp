#include <basalt/gfx/compositor.h>

#include <basalt/api/gfx/drawable.h>
#include <basalt/api/gfx/surface.h>

#include <algorithm>

namespace basalt::gfx {

auto Compositor::compose(ResourceCache& resourceCache,
                         const Surface& drawTarget) -> Composite {
  Composite composite;

  const Drawable::DrawContext context {resourceCache, drawTarget.size()};

  const auto& drawables = drawTarget.drawables();
  std::for_each(
    drawables.begin(), drawables.end(), [&](const DrawablePtr& drawable) {
      composite.emplace_back(drawable->draw(context));
    });

  return composite;
}

} // namespace basalt::gfx
