#pragma once
#ifndef BS_IAPPLICATION_H
#define BS_IAPPLICATION_H

#include "Config.h"

namespace basalt {

struct IApplication {
  IApplication() = default;
  virtual ~IApplication() = default;

  IApplication(const IApplication&) = delete;
  IApplication(IApplication&&) = delete;

  auto operator=(const IApplication&) -> IApplication& = delete;
  auto operator=(IApplication&&) -> IApplication& = delete;

  virtual void on_init() = 0;
  virtual void on_shutdown() = 0;
  virtual void on_update() = 0;

  static auto create(Config& config) -> IApplication*;
};

} // namespace basalt

#endif // !BS_IAPPLICATION_H
