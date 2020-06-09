#pragma once

#include "Input.h"
#include "types.h"

#include "gfx/draw_target.h"
#include "gfx/backend/context.h"
#include "shared/Config.h"

#include <memory>

namespace basalt {

struct Engine {
  Engine() = delete;

  Engine(const Engine&) = delete;
  Engine(Engine&&) = delete;

  ~Engine() = default;

  auto operator=(const Engine&) -> Engine& = delete;
  auto operator=(Engine&&) -> Engine& = delete;

  [[nodiscard]]
  auto config() const -> const Config&;

  [[nodiscard]]
  auto gfx_context() const -> gfx::backend::IGfxContext&;

  [[nodiscard]]
  auto mouse_cursor() const -> MouseCursor;
  void set_mouse_cursor(MouseCursor);

protected:
  Config& mConfig;
  std::shared_ptr<gfx::backend::IGfxContext> mGfxContext {};
  MouseCursor mMouseCursor {};
  bool mIsDirty {false};

  Engine(Config&, std::shared_ptr<gfx::backend::IGfxContext>);
};

struct UpdateContext final {
  Engine& engine;
  gfx::DrawTarget& drawTarget;
  f64 deltaTime {};
  Input input {};
};

void quit();

} // namespace basalt
