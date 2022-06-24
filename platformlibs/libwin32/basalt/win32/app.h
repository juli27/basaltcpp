#pragma once

#include <basalt/runtime.h>

#include <basalt/win32/shared/Windows_custom.h>

#include <basalt/gfx/backend/types.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/shared/types.h>

namespace basalt {

struct App final : Runtime {
  static void run(Config&, HMODULE, int showCommand);

private:
  App(Config&, gfx::Info, gfx::ContextPtr, DearImGuiPtr);
};

} // namespace basalt
