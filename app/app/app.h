#pragma once

#include "shared/Windows_custom.h"

#include <runtime/engine.h>

namespace basalt::win32 {

struct App final : Engine {
  using Engine::Engine;

  static void run(HMODULE, int showCommand);
};

} // namespace basalt::win32
