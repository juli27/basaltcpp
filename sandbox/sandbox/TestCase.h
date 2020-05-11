#pragma once
#ifndef TESTCASE_H
#define TESTCASE_H

#include "runtime/shared/Types.h"

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
};

#endif // !TESTCASE_H
