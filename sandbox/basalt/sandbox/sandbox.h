#pragma once

#include "settings.h"

#include <basalt/api/view.h>

#include <basalt/api/debug_ui.h>
#include <basalt/api/types.h>

#include <basalt/api/base/types.h>

#include <vector>

struct SandboxView final : basalt::View {
  static auto create(basalt::Engine&, Settings) -> basalt::ViewPtr;

  SandboxView(basalt::Engine&, Settings);

protected:
  auto on_update(UpdateContext&) -> void override;

private:
  struct Example;

  std::vector<Example> mExamples;
  basalt::uSize mCurrentExampleIndex{0};
  basalt::ViewPtr mCurrentExampleView;

  basalt::DebugUi mDebugUi{};
  Settings mSettings;

  bool mShowSettingsEditor{false};
  bool mShowDemo{false};
  bool mShowMetrics{false};
  bool mShowAbout{false};
  bool mShowOverlay{true};

  auto reload_scene(basalt::Engine&) -> void;
  auto next_scene(basalt::Engine&) -> void;
  auto prev_scene(basalt::Engine&) -> void;
  auto set_scene(basalt::uSize index, basalt::Engine&) -> void;
};
