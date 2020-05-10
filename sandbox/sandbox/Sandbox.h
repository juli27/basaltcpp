#pragma once
#ifndef SANDBOX_H
#define SANDBOX_H

#include <runtime/IApplication.h>
#include <runtime/shared/Types.h>

#include <memory>
#include <vector>

struct TestCase;

struct SandboxApp final : basalt::IApplication {
  SandboxApp();

  SandboxApp(const SandboxApp&) = delete;
  SandboxApp(SandboxApp&&) = delete;

  ~SandboxApp() override;

  auto operator=(const SandboxApp&) -> SandboxApp& = delete;
  auto operator=(SandboxApp&&) -> SandboxApp& = delete;

  void on_update() override;

private:
  std::vector<std::unique_ptr<TestCase>> mScenes {};
  basalt::i32 mCurrentSceneIndex {0};

  void next_scene();
  void prev_scene();
  void set_scene(basalt::i32 index);
};

#endif // !SANDBOX_H
