#pragma once
#ifndef BS_IAPPLICATION_H
#define BS_IAPPLICATION_H

#include "Config.h"
#include "gfx/backend/IRenderer.h"

namespace basalt {

class IApplication {
public:
  virtual ~IApplication() = default;

  virtual void OnInit(gfx::backend::IRenderer* renderer) = 0;

  virtual void OnShutdown() = 0;

  virtual void OnUpdate() = 0;

public:
  static auto Create(Config& config) -> IApplication*;
};

} // namespace basalt

#endif // !BS_IAPPLICATION_H
