#pragma once

#include <basalt/api/engine.h>

#include <basalt/win32/shared/Windows_custom.h>

namespace basalt {

struct App final : Engine {
  static void run(Config&, HMODULE, int showCommand);

private:
  App(Config&, gfx::Device&);
};

} // namespace basalt
