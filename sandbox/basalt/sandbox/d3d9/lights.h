#pragma once

#include <basalt/sandbox/test_case.h>

#include <memory>

namespace d3d9 {

struct Lights final : TestCase {
  explicit Lights(basalt::Engine&);

  auto drawable() -> basalt::gfx::DrawablePtr override;

  void tick(basalt::Engine&) override;

private:
  struct MyDrawable;

  std::shared_ptr<MyDrawable> mDrawable;
};

} // namespace d3d9
