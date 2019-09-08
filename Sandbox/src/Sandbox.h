#pragma once
#ifndef SANDBOX_H
#define SANDBOX_H

#include <memory>
#include <vector>

#include <Basalt.h>

struct ITestCase;

struct SandboxApp final : bs::IApplication {
  explicit SandboxApp(bs::Config& config);
  SandboxApp(const SandboxApp&) = delete;
  SandboxApp(SandboxApp&&) = delete;
  virtual ~SandboxApp() = default;

  auto operator=(const SandboxApp&) -> SandboxApp& = delete;
  auto operator=(SandboxApp&&) -> SandboxApp& = delete;

  void NextScene();
  void PrevScene();
  void SetScene(bs::i32 index);

  void on_init() override;
  void on_shutdown() override;
  void on_update() override;

private:
  std::vector<std::unique_ptr<ITestCase>> mScenes;
  bs::i32 mCurrentSceneIndex = 0u;
};

#endif // !SANDBOX_H
