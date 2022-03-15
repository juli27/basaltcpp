#pragma once

#include <basalt/api/view.h>

namespace d3d9 {

struct Device final : basalt::View {
  Device() noexcept = default;

private:
  void on_draw(const DrawContext&) override;
};

} // namespace d3d9
