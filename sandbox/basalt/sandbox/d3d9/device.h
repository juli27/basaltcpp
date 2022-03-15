#pragma once

#include <basalt/api/view.h>

namespace d3d9 {

struct Device final : basalt::View {
  Device() noexcept = default;

private:
  auto on_draw(const DrawContext&) -> basalt::gfx::CommandList override;
};

} // namespace d3d9
