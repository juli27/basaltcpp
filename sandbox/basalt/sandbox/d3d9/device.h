#pragma once

#include <basalt/sandbox/test_case.h>

#include <basalt/api/gfx/types.h>

namespace d3d9 {

struct Device final : TestCase {
  Device();

  Device(const Device&) = delete;
  Device(Device&&) = delete;

  ~Device() override = default;

  auto operator=(const Device&) -> Device& = delete;
  auto operator=(Device &&) -> Device& = delete;

  void on_update(const basalt::UpdateContext&) override;
  auto name() -> std::string_view override;

private:
  basalt::gfx::DrawablePtr mDrawable;
};

} // namespace d3d9
