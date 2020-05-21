#pragma once
#ifndef BASALT_RUNTIME_DEBUG_H
#define BASALT_RUNTIME_DEBUG_H

namespace basalt {

struct Scene;

struct Debug final {
  Debug() = delete;

  Debug(const Debug&) = delete;
  Debug(const Debug&&) = delete;

  ~Debug() = delete;

  auto operator=(const Debug&) -> Debug& = delete;
  auto operator=(Debug&&) -> Debug& = delete;

  static void draw_scene_debug_ui(bool* open, Scene*);

private:

};

} // namespace basalt

#endif // BASALT_RUNTIME_DEBUG_H