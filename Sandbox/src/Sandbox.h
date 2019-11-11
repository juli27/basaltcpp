#pragma once
#ifndef SANDBOX_H
#define SANDBOX_H

#include <basalt/Basalt.h>

#include <memory>
#include <vector>

struct ITestCase;

struct SandboxApp final : basalt::IApplication {
  explicit SandboxApp(basalt::Config& config);
  SandboxApp(const SandboxApp&) = delete;
  SandboxApp(SandboxApp&&) = delete;
  virtual ~SandboxApp() = default;

  auto operator=(const SandboxApp&) -> SandboxApp& = delete;
  auto operator=(SandboxApp&&) -> SandboxApp& = delete;

  void next_scene();
  void prev_scene();
  void set_scene(basalt::i32 index);

  void on_init() override;
  void on_shutdown() override;
  void on_update() override;

private:
  std::vector<std::unique_ptr<ITestCase>> mScenes;
  basalt::i32 mCurrentSceneIndex = 0u;
};

#endif // !SANDBOX_H
