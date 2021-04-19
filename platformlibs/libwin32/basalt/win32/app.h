#pragma once

#include <basalt/api/engine.h>

#include <basalt/win32utilities/Windows_custom.h>

namespace basalt {

struct App final : Engine {
  static void run(HMODULE, int showCommand);

private:
  App(Config&, gfx::ContextPtr);
};

} // namespace basalt
