#pragma once

#include <Basalt.h>

class SandboxApp : public bs::IApplication {
public:
  SandboxApp(bs::Config& config);

  virtual ~SandboxApp() = default;

  virtual void OnInit(bs::gfx::backend::IRenderer* renderer) override;

  virtual void OnShutdown() override;

  virtual void OnUpdate(double elapsedTimeInSeconds) override;

private:
  bs::gfx::backend::IRenderer* m_renderer;
  bs::gfx::backend::RenderCommand m_triangleCommand;
};