#pragma once
#ifndef SANDBOX_H
#define SANDBOX_H

#include <memory>
#include <vector>

#include <Basalt.h>

#include "IScene.h"

class SandboxApp : public bs::IApplication {
public:
  SandboxApp(bs::Config& config);

  virtual ~SandboxApp() = default;

  virtual void OnInit(bs::gfx::backend::IRenderer* renderer) override;

  virtual void OnShutdown() override;

  virtual void OnUpdate() override;

private:
  std::vector<std::unique_ptr<IScene>> mScenes;
  bs::i32 mCurrentSceneIndex;
};

#endif // !SANDBOX_H
