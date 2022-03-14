#pragma once

#include <basalt/api/view.h>

namespace d3d9 {

struct Device final : basalt::View {
  Device() noexcept = default;

private:
  auto on_draw(basalt::gfx::ResourceCache&, basalt::Size2Du16 viewport,
               const basalt::RectangleU16& clip)
    -> std::tuple<basalt::gfx::CommandList, basalt::RectangleU16> override;
};

} // namespace d3d9
