#pragma once

namespace basalt {

struct Scene;

namespace gfx {

struct Composite;

} // namespace gfx

// TODO: split up into one Debug class for each subsystem (Scene, Gfx,...)
struct Debug final {
  Debug() = delete;

  Debug(const Debug&) = delete;
  Debug(Debug&&) = delete;

  ~Debug() = delete;

  auto operator=(const Debug&) -> Debug& = delete;
  auto operator=(Debug &&) -> Debug& = delete;

  static void update(Scene&);
  static void update(const gfx::Composite&);

private:
  static void draw_scene_debug_ui(Scene&);
};

} // namespace basalt