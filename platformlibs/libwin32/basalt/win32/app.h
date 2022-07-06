#pragma once

#include <basalt/runtime.h>

#include <basalt/win32/shared/Windows_custom.h>

#include <basalt/gfx/backend/types.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/shared/types.h>

namespace basalt {

class App final : public Runtime {
public:
  static auto run(Config&, HMODULE, int showCommand) -> void;

private:
  App(Config&, gfx::Info, gfx::ContextPtr, DearImGuiPtr);
};

} // namespace basalt
