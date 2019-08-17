#pragma once
#ifndef SANDBOX_H
#define SANDBOX_H

#include <memory>
#include <vector>

#include <Basalt.h>

#include "ITestCase.h"

class SandboxApp final : public bs::IApplication {
public:
  explicit SandboxApp(bs::Config& config);

  virtual ~SandboxApp() = default;

  SandboxApp(const SandboxApp&) = delete;
  SandboxApp(SandboxApp&&) = delete;

public:
  auto operator=(const SandboxApp&) -> SandboxApp& = delete;
  auto operator=(SandboxApp&&) -> SandboxApp& = delete;

public:
  virtual void OnInit() override;
  virtual void OnShutdown() override;
  virtual void OnUpdate() override;

private:
  std::vector<std::unique_ptr<ITestCase>> mScenes;
  bs::i32 mCurrentSceneIndex;
};

#endif // !SANDBOX_H
