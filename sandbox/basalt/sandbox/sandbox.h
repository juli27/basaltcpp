#pragma once

#include <basalt/api/client_app.h>
#include <basalt/api/view.h>

#include <basalt/api/base/types.h>

#include <memory>
#include <string>
#include <vector>

struct SandboxView;
struct TestCase;

struct SandboxApp final : basalt::ClientApp {
  explicit SandboxApp(basalt::Engine&);

  void on_update(basalt::Engine&) override;

private:
  std::shared_ptr<SandboxView> mSandboxView;
};

struct SandboxView final : basalt::View {
  explicit SandboxView(basalt::Engine&);

  void tick(basalt::Engine&) override;

protected:
  auto do_handle_input(const basalt::InputEvent&)
    -> basalt::InputEventHandled override;

private:
  struct Example final {
    std::shared_ptr<TestCase> view;
    std::string name;
  };

  std::vector<Example> mExamples;
  basalt::uSize mCurrentExampleIndex {9};

  void next_scene(basalt::Engine&) noexcept;
  void prev_scene(basalt::Engine&) noexcept;
  void set_scene(basalt::uSize index, basalt::Engine&) noexcept;
};
