#include "Gfx.h"

#include "scene_view.h"
#include "backend/IRenderer.h"

#include <runtime/shared/Asserts.h>

namespace basalt::gfx {

void render(backend::IRenderer* renderer, const SceneView& view) {
  BASALT_ASSERT(renderer);

  renderer->set_clear_color(view.clear_color());
  renderer->render(view.draw());
}

} // namespace basalt::gfx
