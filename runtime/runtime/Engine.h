#pragma once
#ifndef BASALT_ENGINE_H
#define BASALT_ENGINE_H

#include "Input.h"
#include "types.h"

#include "shared/Size2D.h"

namespace basalt {

namespace gfx {

struct View;

} // namespace gfx

struct Engine {
  MouseCursor mouseCursor {};

  Engine() = default;

  Engine(const Engine&) = delete;
  Engine(Engine&&) = delete;

  ~Engine() = default;

  auto operator=(const Engine&) -> Engine& = delete;
  auto operator=(Engine&&) -> Engine& = delete;
};

struct UpdateContext final {
  Engine& engine;

  f64 deltaTime {};
  Size2Du16 windowSize {Size2Du16::dont_care()};
  Input input {};
};

void quit();

void set_view(const gfx::View&);

} // namespace basalt

#endif // !BASALT_ENGINE_H
