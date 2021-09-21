#pragma once

#include <basalt/api/client_app.h>
#include <basalt/api/layer.h>

#include <basalt/api/base/types.h>

#include <memory>
#include <vector>

struct SandboxLayer;
struct TestCase;

struct SandboxApp final : basalt::ClientApp {
  explicit SandboxApp(basalt::Engine&);

  void on_update(basalt::Engine&) override;

private:
  std::shared_ptr<SandboxLayer> mSandboxLayer;
};

struct SandboxLayer final : basalt::Layer {
  explicit SandboxLayer(basalt::Engine&);

  void tick(basalt::Engine&) override;

protected:
  auto do_handle_input(const basalt::InputEvent&)
    -> basalt::InputEventHandled override;

private:
  std::vector<std::unique_ptr<TestCase>> mScenes;
  basalt::uSize mCurrentSceneIndex {9};

  void next_scene(basalt::Engine&) noexcept;
  void prev_scene(basalt::Engine&) noexcept;
  void set_scene(basalt::uSize index, basalt::Engine&) noexcept;
};
