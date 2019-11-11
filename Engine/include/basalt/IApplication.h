#pragma once
#ifndef BASALT_IAPPLICATION_H
#define BASALT_IAPPLICATION_H

#include "Config.h"

namespace basalt {

struct IApplication {
  IApplication() = default;
  IApplication(const IApplication&) = delete;
  IApplication(IApplication&&) = delete;
  virtual ~IApplication() = default;

  auto operator=(const IApplication&) -> IApplication& = delete;
  auto operator=(IApplication&&) -> IApplication& = delete;

  virtual void on_init() = 0;
  virtual void on_shutdown() = 0;
  virtual void on_update() = 0;

  static auto create(Config& config) -> IApplication*;
};

} // namespace basalt

#endif // !BASALT_IAPPLICATION_H
