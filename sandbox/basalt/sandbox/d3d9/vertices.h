#pragma once

#include <basalt/sandbox/test_case.h>

namespace d3d9 {

struct Vertices final : TestCase {
  explicit Vertices(basalt::Engine&);

  Vertices(const Vertices&) = delete;
  Vertices(Vertices&&) = delete;

  ~Vertices() override = default;

  auto operator=(const Vertices&) -> Vertices& = delete;
  auto operator=(Vertices &&) -> Vertices& = delete;

  auto name() -> std::string_view override;
  auto drawable() -> basalt::gfx::DrawablePtr override;

  void on_update(const basalt::UpdateContext&) override;

private:
  basalt::gfx::DrawablePtr mDrawable;
};

} // namespace d3d9
