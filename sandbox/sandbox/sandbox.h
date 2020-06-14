#pragma once

#include <runtime/client_app.h>

#include <runtime/shared/types.h>

#include <memory>
#include <vector>

struct TestCase;

struct SandboxApp final : basalt::ClientApp {
  SandboxApp() = delete;
  explicit SandboxApp(basalt::Engine&);

  SandboxApp(const SandboxApp&) = delete;
  SandboxApp(SandboxApp&&) = delete;

  ~SandboxApp() override = default;

  auto operator=(const SandboxApp&) -> SandboxApp& = delete;
  auto operator=(SandboxApp&&) -> SandboxApp& = delete;

  void on_update(const basalt::UpdateContext&) override;

private:
  std::vector<std::unique_ptr<TestCase>> mScenes {};
  basalt::uSize mCurrentSceneIndex {6};

  void next_scene();
  void prev_scene();
  void set_scene(basalt::uSize index);
};
