#pragma once

#include <basalt/api/view.h>

#include <basalt/api/types.h>

namespace d3d9 {

struct Device final : basalt::View {
  explicit Device(basalt::Engine&) noexcept;

private:
  auto on_draw(const DrawContext&) -> void override;
};

} // namespace d3d9
