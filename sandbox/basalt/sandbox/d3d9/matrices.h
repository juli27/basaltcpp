#pragma once

#include <basalt/sandbox/test_case.h>

#include <memory>

namespace d3d9 {

struct Matrices final : TestCase {
  explicit Matrices(basalt::Engine&);

  auto drawable() -> basalt::gfx::DrawablePtr override;

private:
  struct MyDrawable;

  std::shared_ptr<MyDrawable> mDrawable;

  void on_tick(basalt::Engine&) override;
};

} // namespace d3d9
