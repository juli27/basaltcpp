#pragma once

#include <basalt/api/scene/types.h>

namespace basalt {

// TODO: split up into one Debug class for each subsystem (Scene, Gfx,...)
struct Debug final {
  Debug() = delete;

  static void update(Scene&);

private:
  static void draw_scene_debug_ui(Scene&);
};

} // namespace basalt
