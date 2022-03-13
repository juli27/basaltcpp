#pragma once

#include <basalt/sandbox/test_case.h>

namespace d3d9 {

struct Vertices final : TestCase {
  explicit Vertices(basalt::Engine&);

  auto drawable() -> basalt::gfx::DrawablePtr override;

  void on_tick(basalt::Engine&) override;

private:
  basalt::gfx::DrawablePtr mDrawable;
};

} // namespace d3d9
