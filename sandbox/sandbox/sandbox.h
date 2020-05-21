#pragma once
#ifndef SANDBOX_H
#define SANDBOX_H

#include <runtime/client_app.h>
#include <runtime/shared/Types.h>

#include <memory>
#include <vector>

struct TestCase;

struct SandboxApp final : basalt::ClientApp {
  SandboxApp() = delete;
  explicit SandboxApp(basalt::gfx::backend::IRenderer*);

  SandboxApp(const SandboxApp&) = delete;
  SandboxApp(SandboxApp&&) = delete;

  ~SandboxApp() override;

  auto operator=(const SandboxApp&) -> SandboxApp& = delete;
  auto operator=(SandboxApp&&) -> SandboxApp& = delete;

  void on_update(basalt::f64 deltaTime) override;

private:
  std::vector<std::unique_ptr<TestCase>> mScenes {};
  basalt::uSize mCurrentSceneIndex {5};

  void next_scene();
  void prev_scene();
  void set_scene(basalt::uSize index);
};

#endif // !SANDBOX_H
