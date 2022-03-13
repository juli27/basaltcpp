#pragma once

#include <basalt/sandbox/test_case.h>

#include <basalt/api/gfx/types.h>

namespace d3d9 {

struct Device final : TestCase {
  Device();

  auto drawable() -> basalt::gfx::DrawablePtr override;

  void on_tick(basalt::Engine&) override;

private:
  basalt::gfx::DrawablePtr mDrawable;
};

} // namespace d3d9
