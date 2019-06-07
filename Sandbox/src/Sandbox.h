#pragma once

#include <Basalt.h>

#include "scenes/d3d9/tutorials/Vertices.h"

class SandboxApp : public bs::IApplication {
public:
  SandboxApp(bs::Config& config);

  virtual ~SandboxApp() = default;

  virtual void OnInit(bs::gfx::backend::IRenderer* renderer) override;

  virtual void OnShutdown() override;

  virtual void OnUpdate(double elapsedTimeInSeconds) override;

private:
  VerticesScene* m_verticesScene;
};