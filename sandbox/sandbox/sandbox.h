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
  explicit SandboxApp(
    basalt::gfx::backend::IRenderer*, basalt::Size2Du16 windowSize);

  SandboxApp(const SandboxApp&) = delete;
  SandboxApp(SandboxApp&&) = delete;

  ~SandboxApp() override;

  auto operator=(const SandboxApp&) -> SandboxApp& = delete;
  auto operator=(SandboxApp&&) -> SandboxApp& = delete;

  void on_update(const basalt::UpdateContext&) override;

private:
  std::vector<std::unique_ptr<TestCase>> mScenes {};
  basalt::uSize mCurrentSceneIndex {6};

  void next_scene(basalt::Size2Du16 windowSize);
  void prev_scene(basalt::Size2Du16 windowSize);
  void set_scene(basalt::uSize index, basalt::Size2Du16 windowSize);
};

#endif // !SANDBOX_H
