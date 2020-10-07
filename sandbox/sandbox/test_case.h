#pragma once

#include <api/engine.h>

#include <string_view>

struct TestCase {
  TestCase(const TestCase&) = delete;
  TestCase(TestCase&&) = delete;

  virtual ~TestCase() noexcept = default;

  auto operator=(const TestCase&) -> TestCase& = delete;
  auto operator=(TestCase &&) -> TestCase& = delete;

  virtual void on_update(const basalt::UpdateContext&) = 0;
  virtual auto name() -> std::string_view = 0;

protected:
  TestCase() noexcept = default;
};
