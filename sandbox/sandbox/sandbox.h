#pragma once
#ifndef SANDBOX_H
#define SANDBOX_H

#include <runtime/client_app.h>

#include <runtime/shared/Size2D.h>
#include <runtime/shared/Types.h>

#include <memory>
#include <vector>

struct TestCase;

struct SandboxApp final : basalt::ClientApp {
  SandboxApp() = delete;
  explicit SandboxApp(basalt::Engine&, basalt::Size2Du16 windowSize);

  SandboxApp(const SandboxApp&) = delete;
  SandboxApp(SandboxApp&&) = delete;

  ~SandboxApp() override = default;

  auto operator=(const SandboxApp&) -> SandboxApp& = delete;
  auto operator=(SandboxApp&&) -> SandboxApp& = delete;

  void on_update(const basalt::UpdateContext&) override;

private:
  std::vector<std::unique_ptr<TestCase>> mScenes {};
  basalt::uSize mCurrentSceneIndex {6};

  void next_scene(const basalt::UpdateContext&);
  void prev_scene(const basalt::UpdateContext&);
  void set_scene(const basalt::UpdateContext&, basalt::uSize index);
};

#endif // !SANDBOX_H
