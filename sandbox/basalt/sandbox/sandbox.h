#pragma once

#include <basalt/api/view.h>

#include <basalt/api/base/types.h>

#include <vector>

struct SandboxView final : basalt::View {
  explicit SandboxView(basalt::Engine&);

private:
  struct Example;

  std::vector<Example> mExamples;
  basalt::uSize mCurrentExampleIndex {9};

  void next_scene(basalt::Engine&) noexcept;
  void prev_scene(basalt::Engine&) noexcept;
  void set_scene(basalt::uSize index, basalt::Engine&) noexcept;

  auto on_input(const basalt::InputEvent&)
    -> basalt::InputEventHandled override;

  void on_tick(basalt::Engine&) override;
};
