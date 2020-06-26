#pragma once

#include <runtime/engine.h>

#include "shared/Windows_custom.h"

#include <memory>

namespace basalt {

struct Window;

namespace gfx {

struct Context;

} // namespace gfx

struct App final : Engine {
  App() = delete;

  App(const App&) = delete;
  App(App&&) = delete;

  auto operator=(const App&) -> App& = delete;
  auto operator=(App&&) -> App& = delete;

  static void run(HMODULE, int showCommand);

private:
  App(Config&, std::shared_ptr<gfx::Context>);

  ~App() = default;
};

} // namespace basalt
