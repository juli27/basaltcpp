#pragma once
#ifndef BASALT_RUNTIME_DEBUG_H
#define BASALT_RUNTIME_DEBUG_H

namespace basalt {

struct Scene;

namespace gfx {

struct View;

} // namespace gfx

struct Debug final {
  Debug() = delete;

  Debug(const Debug&) = delete;
  Debug(const Debug&&) = delete;

  ~Debug() = delete;

  auto operator=(const Debug&) -> Debug& = delete;
  auto operator=(Debug&&) -> Debug& = delete;

  static void update(const gfx::View&);

private:
  static bool sShowSceneDebugUi;
  static bool sShowDemo;
  static bool sShowMetrics;
  static bool sShowAbout;

  static void draw_scene_debug_ui(Scene*);
};

} // namespace basalt

#endif // BASALT_RUNTIME_DEBUG_H