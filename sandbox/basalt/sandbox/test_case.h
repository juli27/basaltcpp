#pragma once

#include <basalt/api/types.h>
#include <basalt/api/gfx/types.h>

#include <string_view>

struct TestCase {
  TestCase(const TestCase&) = delete;
  TestCase(TestCase&&) = delete;

  virtual ~TestCase() noexcept = default;

  auto operator=(const TestCase&) -> TestCase& = delete;
  auto operator=(TestCase&&) -> TestCase& = delete;

  [[nodiscard]] virtual auto name() -> std::string_view = 0;
  [[nodiscard]] virtual auto drawable() -> basalt::gfx::DrawablePtr = 0;

  virtual void on_update(const basalt::UpdateContext&) = 0;

protected:
  TestCase() noexcept = default;
};
