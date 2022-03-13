#pragma once

#include <basalt/sandbox/test_case.h>

namespace d3d9 {

struct Device final : TestCase {
  Device() noexcept = default;

private:
  auto on_draw(basalt::gfx::ResourceCache&, basalt::Size2Du16 viewport,
               const basalt::RectangleU16& clip)
    -> std::tuple<basalt::gfx::CommandList, basalt::RectangleU16> override;

  void on_tick(basalt::Engine&) override;
};

} // namespace d3d9
