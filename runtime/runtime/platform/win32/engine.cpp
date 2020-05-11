#include "runtime/Engine.h"

#include "runtime/platform/win32/globals.h"

using std::shared_ptr;

namespace basalt {

using namespace win32;

void set_current_scene(shared_ptr<Scene> scene) {
  sCurrentScene.swap(scene);
}

auto get_current_scene() -> Scene* {
  return sCurrentScene.get();
}


} // namespace basalt
