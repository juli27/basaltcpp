#pragma once

#include <basalt/sandbox/test_case.h>

namespace d3d9 {

struct Device final : TestCase {
  Device();

  Device(const Device&) = delete;
  Device(Device&&) = delete;

  ~Device() override = default;

  auto operator=(const Device&) -> Device& = delete;
  auto operator=(Device &&) -> Device& = delete;

  auto name() -> std::string_view override;
  auto drawable() -> basalt::gfx::DrawablePtr override;

  void on_update(const basalt::UpdateContext&) override;

private:
  basalt::gfx::DrawablePtr mDrawable;
};

} // namespace d3d9
