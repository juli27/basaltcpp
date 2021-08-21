#pragma once

#include <basalt/api/client_app.h>

#include <basalt/api/base/types.h>

#include <memory>
#include <vector>

struct TestCase;

struct SandboxApp final : basalt::ClientApp {
  explicit SandboxApp(basalt::Engine&);

  SandboxApp(const SandboxApp&) = delete;
  SandboxApp(SandboxApp&&) = delete;

  ~SandboxApp() override = default;

  auto operator=(const SandboxApp&) -> SandboxApp& = delete;
  auto operator=(SandboxApp&&) -> SandboxApp& = delete;

  void on_update(basalt::Engine&) override;

private:
  struct Input;

  std::shared_ptr<Input> mInput;
  std::vector<std::unique_ptr<TestCase>> mScenes;
  basalt::uSize mCurrentSceneIndex {9};

  void next_scene(basalt::Engine&) noexcept;
  void prev_scene(basalt::Engine&) noexcept;
  void set_scene(basalt::uSize index, basalt::Engine&) noexcept;
};
