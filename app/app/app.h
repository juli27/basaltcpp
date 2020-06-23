#pragma once

#include <runtime/engine.h>

#include "shared/Windows_custom.h"

namespace basalt {

struct Window;

struct App final : Engine {
  App() = delete;

  App(const App&) = delete;
  App(App&&) = delete;

  auto operator=(const App&) -> App& = delete;
  auto operator=(App&&) -> App& = delete;

  static void run(HMODULE, int showCommand);

private:
  App(Config&, const Window&);

  ~App() = default;
};

} // namespace basalt
