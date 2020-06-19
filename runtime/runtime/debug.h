#pragma once

namespace basalt {

struct Scene;

struct Debug final {
  Debug() = delete;

  Debug(const Debug&) = delete;
  Debug(Debug&&) = delete;

  ~Debug() = delete;

  auto operator=(const Debug&) -> Debug& = delete;
  auto operator=(Debug&&) -> Debug& = delete;

  static void update(Scene&);

private:
  static void draw_scene_debug_ui(Scene&);
};

} // namespace basalt
