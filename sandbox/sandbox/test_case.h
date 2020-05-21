#pragma once
#ifndef SANDBOX_TEST_CASE_H
#define SANDBOX_TEST_CASE_H

#include "runtime/shared/Types.h"

#include <string_view>

struct TestCase {
  TestCase() noexcept = default;

  TestCase(const TestCase&) = delete;
  TestCase(TestCase&&) = delete;

  virtual ~TestCase() = default;

  auto operator=(const TestCase&) -> TestCase& = delete;
  auto operator=(TestCase&&) -> TestCase& = delete;

  virtual void on_show() = 0;
  virtual void on_hide() = 0;
  virtual void on_update(basalt::f64 deltaTime) = 0;
  virtual auto name() -> std::string_view = 0;
};

#endif // !SANDBOX_TEST_CASE_H
