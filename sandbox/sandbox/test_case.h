#pragma once

#include <runtime/engine.h>

#include <string_view>

struct TestCase {
  TestCase() noexcept = default;

  TestCase(const TestCase&) = delete;
  TestCase(TestCase&&) = delete;

  virtual ~TestCase() = default;

  auto operator=(const TestCase&) -> TestCase& = delete;
  auto operator=(TestCase &&) -> TestCase& = delete;

  virtual void on_update(const basalt::UpdateContext&) = 0;
  virtual auto name() -> std::string_view = 0;
};
