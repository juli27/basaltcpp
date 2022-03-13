#pragma once

#include <basalt/sandbox/test_case.h>

namespace d3d9 {

struct Vertices final : TestCase {
  explicit Vertices(basalt::Engine&);

  auto drawable() -> basalt::gfx::DrawablePtr override;

private:
  basalt::gfx::DrawablePtr mDrawable;

  void on_tick(basalt::Engine&) override;
};

} // namespace d3d9
