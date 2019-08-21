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

  virtual void OnShow() = 0;
  virtual void OnHide() = 0;
  virtual void OnUpdate() = 0;
};

#endif // !ITESTCASE_H
