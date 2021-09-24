#pragma once

#include <basalt/sandbox/test_case.h>

namespace d3d9 {

struct Device final : TestCase {
  Device();

  auto name() -> std::string_view override;
  auto drawable() -> basalt::gfx::DrawablePtr override;

  void tick(basalt::Engine&) override;

private:
  basalt::gfx::DrawablePtr mDrawable;
};

} // namespace d3d9
