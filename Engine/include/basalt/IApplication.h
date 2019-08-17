#pragma once
#ifndef BS_IAPPLICATION_H
#define BS_IAPPLICATION_H

#include "Config.h"

namespace basalt {

class IApplication {
public:
  inline IApplication() = default;
  virtual ~IApplication() = default;

  IApplication(const IApplication&) = delete;
  IApplication(IApplication&&) = delete;

public:
  auto operator=(const IApplication&) -> IApplication& = delete;
  auto operator=(IApplication&&) -> IApplication& = delete;

public:

  virtual void OnInit() = 0;


  virtual void OnShutdown() = 0;


  virtual void OnUpdate() = 0;

public:

  static auto Create(Config& config) -> IApplication*;
};

} // namespace basalt

#endif // !BS_IAPPLICATION_H
