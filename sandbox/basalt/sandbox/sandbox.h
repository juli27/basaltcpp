#pragma once

#include <basalt/api/view.h>

#include <basalt/api/debug_ui.h>
#include <basalt/api/types.h>

#include <basalt/api/base/types.h>

#include <vector>

struct SandboxView final : basalt::View {
  explicit SandboxView(basalt::Engine&);

private:
  struct Example;

  std::vector<Example> mExamples;
  basalt::uSize mCurrentExampleIndex {10};
  basalt::ViewPtr mCurrentExampleView;

  basalt::DebugUi mDebugUi {};

  bool mShowDemo {false};
  bool mShowMetrics {false};
  bool mShowAbout {false};
  bool mShowOverlay {true};

  auto next_scene(basalt::Engine&) -> void;
  auto prev_scene(basalt::Engine&) -> void;
  auto switch_scene(basalt::uSize index, basalt::Engine&) -> void;

  auto on_tick(basalt::Engine&) -> void override;
};
