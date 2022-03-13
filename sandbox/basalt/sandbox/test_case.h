#pragma once

#include <basalt/api/view.h>
#include <basalt/api/gfx/drawable.h>

struct TestCase : basalt::View, basalt::gfx::Drawable {
  TestCase(const TestCase&) = delete;
  TestCase(TestCase&&) noexcept = default;

  ~TestCase() noexcept override = default;

  auto operator=(const TestCase&) -> TestCase& = delete;
  auto operator=(TestCase&&) noexcept -> TestCase& = default;

protected:
  TestCase() noexcept = default;
};
