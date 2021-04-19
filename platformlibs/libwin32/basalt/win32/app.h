#pragma once

#include <basalt/api/engine.h>

#include <basalt/win32utilities/Windows_custom.h>

namespace basalt {

struct Window;

namespace gfx {

struct Context;

} // namespace gfx

struct App final : Engine {
  static void run(HMODULE, int showCommand);

private:
  App(Config&, std::shared_ptr<gfx::Context>);
};

} // namespace basalt
