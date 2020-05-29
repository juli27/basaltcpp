#pragma once
#ifndef SANDBOX_TEST_CASE_H
#define SANDBOX_TEST_CASE_H

#include <runtime/gfx/types.h>

#include "runtime/shared/Size2D.h"
#include "runtime/shared/Types.h"

#include <string_view>

struct TestCase {
  TestCase() noexcept = default;

  TestCase(const TestCase&) = delete;
  TestCase(TestCase&&) = delete;

  virtual ~TestCase() = default;

  auto operator=(const TestCase&) -> TestCase& = delete;
  auto operator=(TestCase&&) -> TestCase& = delete;

  virtual auto view(basalt::Size2Du16 windowSize) -> basalt::gfx::View = 0;
  virtual void on_update(basalt::f64 deltaTime) = 0;
  virtual auto name() -> std::string_view = 0;
};

#endif // !SANDBOX_TEST_CASE_H
