#pragma once

#include <basalt/api/view.h>

namespace d3d9 {

struct Device final : basalt::View {
  Device() noexcept = default;

private:
  auto on_draw(basalt::gfx::ResourceCache&, basalt::Size2Du16 viewport)
    -> basalt::gfx::CommandList override;
};

} // namespace d3d9
