#pragma once
#ifndef BASALT_APP_APP_H
#define BASALT_APP_APP_H

#include "shared/Windows_custom.h"

#include <runtime/Engine.h>

namespace basalt::win32 {

struct App final : Engine {
  using Engine::Engine;

  static void run(HMODULE, int showCommand);
};

} // namespace basalt::win32

#endif // BASALT_APP_APP_H
