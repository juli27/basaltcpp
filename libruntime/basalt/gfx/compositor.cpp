#include <basalt/gfx/compositor.h>

#include <basalt/api/gfx/command_list_recorder.h>
#include <basalt/api/gfx/surface.h>
#include <basalt/api/gfx/drawable.h>
#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/math/rectangle.h>

#include <algorithm>
#include <vector>
#include <utility>

namespace basalt::gfx {

auto Compositor::compose(ResourceCache& resourceCache,
                         const Surface& drawTarget) -> Composite {
  Composite composite;

  const Size2Du16 viewport = drawTarget.size();
  RectangleU16 obscuredRegion;

  const auto& drawables = drawTarget.drawables();
  std::for_each(drawables.rbegin(), drawables.rend(),
                [&](const DrawablePtr& drawable) {
                  if (obscuredRegion.area() == viewport.to_rectangle().area()) {
                    return;
                  }

                  auto [commandList, clipRect] =
                    drawable->draw(resourceCache, viewport, obscuredRegion);

                  obscuredRegion = RectangleU16 {
                    std::min(obscuredRegion.left(), clipRect.left()),
                    std::min(obscuredRegion.top(), clipRect.top()),
                    std::max(obscuredRegion.right(), clipRect.right()),
                    std::max(obscuredRegion.bottom(), clipRect.bottom()),
                  };

                  composite.emplace_back(std::move(commandList));
                });

  if (obscuredRegion != viewport.to_rectangle()) {
    CommandListRecorder cmdList;
    cmdList.clear(Colors::BLACK);
    composite.emplace_back(cmdList.take_cmd_list());
  }

  std::reverse(composite.begin(), composite.end());

  return composite;
}

} // namespace basalt::gfx
