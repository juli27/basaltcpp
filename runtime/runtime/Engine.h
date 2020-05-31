#pragma once
#ifndef BASALT_ENGINE_H
#define BASALT_ENGINE_H

#include "Input.h"
#include "types.h"

#include "gfx/types.h"

#include "shared/Config.h"
#include "shared/Size2D.h"

namespace basalt {

namespace gfx::backend {

struct IRenderer;

} // namespace gfx::backend

struct Engine {
  gfx::backend::IRenderer* const renderer {};
  gfx::View currentView {};

  Engine() = delete;

  Engine(const Engine&) = delete;
  Engine(Engine&&) = delete;

  ~Engine() = default;

  auto operator=(const Engine&) -> Engine& = delete;
  auto operator=(Engine&&) -> Engine& = delete;

  auto config() const -> const Config&;

  auto mouse_cursor() const -> MouseCursor;
  void set_mouse_cursor(MouseCursor);

protected:
  Config& mConfig;
  MouseCursor mMouseCursor {};
  bool mIsDirty {false};

  Engine(Config&, gfx::backend::IRenderer*);
};

struct UpdateContext final {
  Engine& engine;

  f64 deltaTime {};
  Size2Du16 windowSize {Size2Du16::dont_care()};
  Input input {};
};

void quit();

} // namespace basalt

#endif // !BASALT_ENGINE_H
