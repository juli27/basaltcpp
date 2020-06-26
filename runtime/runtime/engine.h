#pragma once

#include "input.h"
#include "types.h"

#include "shared/types.h"

#include <memory>

namespace basalt {

struct Config;

namespace gfx {

struct Context;
struct Device;
struct DrawTarget;

} // namespace gfx

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
  auto gfx_device() const -> gfx::Device&;

  [[nodiscard]]
  auto gfx_context() const -> gfx::Context&;

  [[nodiscard]]
  auto mouse_cursor() const -> MouseCursor;
  void set_mouse_cursor(MouseCursor);

protected:
  Config& mConfig;
  std::shared_ptr<gfx::Context> mGfxContext {};
  MouseCursor mMouseCursor {};
  bool mIsDirty {false};

  Engine(Config&, std::shared_ptr<gfx::Context>);
};

struct UpdateContext final {
  Engine& engine;
  gfx::DrawTarget& drawTarget;
  f64 deltaTime {};
  Input input {};
};

void quit();

} // namespace basalt
