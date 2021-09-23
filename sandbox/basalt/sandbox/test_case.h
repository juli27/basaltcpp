#pragma once

#include <basalt/api/view.h>
#include <basalt/api/gfx/types.h>

#include <string_view>

struct TestCase : basalt::View {
  TestCase(const TestCase&) = delete;
  TestCase(TestCase&&) noexcept = default;

  ~TestCase() noexcept override = default;

  auto operator=(const TestCase&) -> TestCase& = delete;
  auto operator=(TestCase&&) noexcept -> TestCase& = default;

  [[nodiscard]] virtual auto name() -> std::string_view = 0;
  [[nodiscard]] virtual auto drawable() -> basalt::gfx::DrawablePtr = 0;

protected:
  TestCase() noexcept = default;
};
