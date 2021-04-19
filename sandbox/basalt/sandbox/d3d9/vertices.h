#pragma once

#include <basalt/sandbox/test_case.h>

#include <basalt/api/gfx/types.h>

namespace d3d9 {

struct Vertices final : TestCase {
  explicit Vertices(basalt::Engine&);

  Vertices(const Vertices&) = delete;
  Vertices(Vertices&&) = delete;

  ~Vertices() override = default;

  auto operator=(const Vertices&) -> Vertices& = delete;
  auto operator=(Vertices &&) -> Vertices& = delete;

  void on_update(const basalt::UpdateContext&) override;
  auto name() -> std::string_view override;

private:
  basalt::gfx::DrawablePtr mDrawable;
};

} // namespace d3d9
