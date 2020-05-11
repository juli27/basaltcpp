#include "runtime/Engine.h"

#include "runtime/platform/win32/globals.h"

#include <utility>

using std::shared_ptr;

namespace basalt {

using namespace win32;

using gfx::backend::IRenderer;

auto get_renderer() -> IRenderer* {
  return sRenderer.get();
}

void set_current_scene(shared_ptr<Scene> scene) {
  sCurrentScene = std::move(scene);
}

auto get_current_scene() -> Scene* {
  return sCurrentScene.get();
}


} // namespace basalt
