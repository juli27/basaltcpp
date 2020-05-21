#include "runtime/Engine.h"

#include "runtime/Scene.h"

#include "runtime/platform/win32/globals.h"
#include "runtime/shared/win32/Windows_custom.h"

using std::shared_ptr;

namespace basalt {

using namespace win32;

void set_current_scene(shared_ptr<Scene> scene) {
  sCurrentScene.swap(scene);
}

auto get_current_scene() -> Scene* {
  return sCurrentScene.get();
}

void draw_scene_debug_ui(bool* open) {
  sCurrentScene->display_debug_gui(open);
}

void quit() {
  ::PostQuitMessage(0);
}

} // namespace basalt
