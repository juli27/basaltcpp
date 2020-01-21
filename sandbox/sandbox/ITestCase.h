#pragma once
#ifndef ITESTCASE_H
#define ITESTCASE_H

struct ITestCase {
  ITestCase() noexcept = default;
  ITestCase(const ITestCase&) = delete;
  ITestCase(ITestCase&&) = delete;
  virtual ~ITestCase() noexcept = default;

  auto operator=(const ITestCase&) -> ITestCase& = delete;
  auto operator=(ITestCase&&) -> ITestCase& = delete;

  virtual void on_show() = 0;
  virtual void on_hide() = 0;
  virtual void on_update() = 0;
};

#endif // !ITESTCASE_H
