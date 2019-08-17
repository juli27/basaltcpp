#pragma once
#ifndef ISCENE_H
#define ISCENE_H

class ITestCase {
public:
  inline ITestCase() = default;
  inline virtual ~ITestCase() noexcept = default;

  ITestCase(const ITestCase&) = delete;
  ITestCase(ITestCase&&) = delete;

public:
  auto operator=(const ITestCase&) -> ITestCase& = delete;
  auto operator=(ITestCase&&) -> ITestCase& = delete;

public:

  virtual void OnShow() = 0;


  virtual void OnHide() = 0;


  virtual void OnUpdate() = 0;
};

#endif // !ISCENE_H
