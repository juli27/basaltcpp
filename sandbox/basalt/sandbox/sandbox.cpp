#include "sandbox.h"

#include "settings_ui.h"

#include "d3d9_tutorials.h"
#include "samples/samples.h"
#include "tribase/tribase_examples.h"

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/shared/config.h>

#include <basalt/api/base/asserts.h>
#include <basalt/api/base/platform.h>

#include <imgui.h>

#include <functional>
#include <memory>
#include <string>
#include <utility>

using std::function;
using std::string;
using namespace std::literals;

using basalt::Config;
using basalt::Engine;
using basalt::Platform;
using basalt::ViewPtr;
using basalt::WindowMode;

struct SandboxView::Example final {
  string name;
  function<ViewPtr(Engine&)> factory;
};

auto SandboxView::create(Engine& engine, Settings settings) -> ViewPtr {
  return std::make_shared<SandboxView>(engine, std::move(settings));
}

SandboxView::SandboxView(Engine& engine, Settings settings)
  : mSettings{std::move(settings)} {
  mExamples.reserve(21);
  mExamples.emplace_back(Example{
    "Tutorial 5: Using Texture Maps"s,
    &D3D9Tutorials::new_textures_tutorial,
  });
  mExamples.emplace_back(Example{
    "Tutorial 6: Using Meshes"s,
    &D3D9Tutorials::new_meshes_tutorial,
  });
  mExamples.emplace_back(Example{
    "Bsp. 02-03: Das erste Dreieck"s,
    &TribaseExamples::new_first_triangle_example,
  });
  mExamples.emplace_back(Example{
    "Bsp. 02-04: Texturen"s,
    &TribaseExamples::new_02_04_textures_example,
  });
  mExamples.emplace_back(Example{
    "Bsp. 02-04: Texturen - Aufgabe 1-3"s,
    &TribaseExamples::new_02_04_textures_exercises,
  });
  mExamples.emplace_back(Example{
    "Bsp. 02-05: Vertex- und Index-Buffer"s,
    &TribaseExamples::new_02_05_buffers_example,
  });
  mExamples.emplace_back(Example{
    "Bsp. 02-05: Vertex- und Index-Buffer - Aufgabe 1-2"s,
    &TribaseExamples::new_02_05_buffers_exercises,
  });
  mExamples.emplace_back(Example{
    "Bsp. 02-06: Nebel"s,
    &TribaseExamples::new_02_06_fog_example,
  });
  mExamples.emplace_back(Example{
    "Bsp. 02-07: Beleuchtung"s,
    &TribaseExamples::new_02_07_lighting_example,
  });
  mExamples.emplace_back(Example{
    "Bsp. 02-08: Alpha Blending"s,
    &TribaseExamples::new_02_08_blending_example,
  });
  mExamples.emplace_back(Example{
    "Bsp. 02-09: Multi-Texturing"s,
    &TribaseExamples::new_02_09_multi_texturing_example,
  });
  mExamples.emplace_back(Example{
    "Bsp. 02-10: Volumentexturen"s,
    &TribaseExamples::new_02_10_volume_textures,
  });
  mExamples.emplace_back(Example{
    "Bsp. 02-11: Environment-Mapping"s,
    &TribaseExamples::new_02_11_env_mapping,
  });
  mExamples.emplace_back(Example{
    "Bsp. 02-12: Bump-Mapping"s,
    &TribaseExamples::new_02_12_bump_mapping,
  });
  mExamples.emplace_back(Example{
    "Bsp. 02-13: Stencil Buffer"s,
    &TribaseExamples::new_02_13_stencil_buffer,
  });
  mExamples.emplace_back(Example{
    "Bsp. 02-14: Effekte"s,
    &TribaseExamples::new_02_14_effects,
  });
  mExamples.emplace_back(Example{
    "Simple View Rendering"s,
    &Samples::new_simple_view_rendering_sample,
  });
  mExamples.emplace_back(Example{
    "Simple Scene"s,
    &Samples::new_simple_scene_sample,
  });
  mExamples.emplace_back(Example{
    "Textures"s,
    &Samples::new_textures_sample,
  });
  mExamples.emplace_back(Example{
    "Cubes"s,
    &Samples::new_cubes_sample,
  });
  mExamples.emplace_back(Example{
    "Lighting"s,
    &Samples::new_lighting_sample,
  });

  set_scene(mCurrentExampleIndex, engine);
}

auto SandboxView::on_update(UpdateContext& ctx) -> void {
  auto& engine = ctx.engine;

  // https://github.com/ocornut/imgui/issues/331
  enum class OpenPopup : u8 { None, GfxInfo };
  auto shouldOpenPopup = OpenPopup::None;

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      for (auto i = uSize{0}; i < mExamples.size(); i++) {
        auto const isCurrent = mCurrentExampleIndex == i;
        if (ImGui::MenuItem(mExamples[i].name.c_str(), nullptr, isCurrent,
                            !isCurrent)) {
          set_scene(i, engine);
        }
      }

      ImGui::Separator();

      if (ImGui::MenuItem("Reload Scene", "Ctrl+R")) {
        reload_scene(engine);
      }

      if (ImGui::MenuItem("Next Scene", "PgDn")) {
        next_scene(engine);
      }
      if (ImGui::MenuItem("Prev Scene", "PgUp")) {
        prev_scene(engine);
      }

      ImGui::Separator();

      if (ImGui::MenuItem("Exit", "Alt+F4")) {
        Platform::quit();
      }

      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View")) {
      auto& config = engine.config();

      auto const canvasModeConfigKey = "window.mode"s;
      auto const currentMode =
        config.get_enum(canvasModeConfigKey, basalt::to_window_mode);
      if (ImGui::MenuItem("Windowed", nullptr,
                          currentMode == WindowMode::Windowed,
                          currentMode != WindowMode::Windowed)) {
        config.set_enum(canvasModeConfigKey, WindowMode::Windowed);
      }
      if (ImGui::MenuItem("Fullscreen", nullptr,
                          currentMode == WindowMode::Fullscreen,
                          currentMode != WindowMode::Fullscreen)) {
        config.set_enum(canvasModeConfigKey, WindowMode::Fullscreen);
      }
      if (ImGui::MenuItem("Fullscreen (Exclusive)", nullptr,
                          currentMode == WindowMode::FullscreenExclusive,
                          currentMode != WindowMode::FullscreenExclusive)) {
        config.set_enum(canvasModeConfigKey, WindowMode::FullscreenExclusive);
      }

      ImGui::Separator();

      if (ImGui::MenuItem("GFX Info...")) {
        shouldOpenPopup = OpenPopup::GfxInfo;
      }

      auto sceneInspectorEnabled =
        config.get_bool("debug.scene_inspector.visible"s);

      ImGui::MenuItem("Scene Inspector", nullptr, &sceneInspectorEnabled);

      config.set_bool("debug.scene_inspector.visible"s, sceneInspectorEnabled);

      ImGui::MenuItem("Performance Overlay", nullptr, &mShowOverlay);
      ImGui::MenuItem("Settings", nullptr, &mShowSettingsEditor);

      if (config.get_bool("runtime.debugUI.enabled"s)) {
        ImGui::Separator();
      }

      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Help")) {
      ImGui::MenuItem("Dear ImGui Demo", nullptr, &mShowDemo);
      ImGui::MenuItem("Dear ImGui Metrics", nullptr, &mShowMetrics);
      ImGui::MenuItem("About Dear ImGui", nullptr, &mShowAbout);
      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }

  if (shouldOpenPopup == OpenPopup::GfxInfo) {
    ImGui::OpenPopup("Gfx Info");
  }

  ImGui::SetNextWindowSize(ImVec2{300.0f, 350.0f}, ImGuiCond_FirstUseEver);
  if (ImGui::BeginPopupModal("Gfx Info", nullptr)) {
    auto const footerHeight = ImGui::GetFrameHeightWithSpacing();

    if (ImGui::BeginChild("##info", ImVec2{0.0f, -footerHeight})) {
      mDebugUi.show_gfx_info(engine.gfx_info());
    }

    ImGui::EndChild();

    if (ImGui::Button("Close", ImVec2{120.0f, 0.0f})) {
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }

  if (mShowSettingsEditor) {
    SettingsUi::show_settings_editor(mSettings, engine.gfx_info(),
                                     &mShowSettingsEditor);
  }

  if (mShowOverlay) {
    mDebugUi.show_performance_overlay(mShowOverlay);
  }

  if (mShowDemo) {
    ImGui::ShowDemoWindow(&mShowDemo);
  }
  if (mShowMetrics) {
    ImGui::ShowMetricsWindow(&mShowMetrics);
  }
  if (mShowAbout) {
    ImGui::ShowAboutWindow(&mShowAbout);
  }

  if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_R,
                      ImGuiInputFlags_RouteGlobal)) {
    reload_scene(engine);
  }

  if (ImGui::Shortcut(ImGuiKey_PageDown, ImGuiInputFlags_RouteGlobal)) {
    next_scene(engine);
  } else if (ImGui::Shortcut(ImGuiKey_PageUp, ImGuiInputFlags_RouteGlobal)) {
    prev_scene(engine);
  }
}

auto SandboxView::reload_scene(Engine& engine) -> void {
  set_scene(mCurrentExampleIndex, engine);
}

auto SandboxView::next_scene(Engine& engine) -> void {
  auto nextSceneIndex = mCurrentExampleIndex + 1;
  if (nextSceneIndex >= mExamples.size()) {
    nextSceneIndex = 0;
  }

  set_scene(nextSceneIndex, engine);
}

auto SandboxView::prev_scene(Engine& engine) -> void {
  auto const prevSceneIndex = [&] {
    if (mCurrentExampleIndex == 0) {
      return mExamples.size() - 1;
    }

    return mCurrentExampleIndex - 1;
  }();

  set_scene(prevSceneIndex, engine);
}

auto SandboxView::set_scene(uSize const index, Engine& engine) -> void {
  BASALT_ASSERT(index < mExamples.size());

  if (mExamples.empty()) {
    return;
  }

  remove_child(mCurrentExampleView);

  mCurrentExampleIndex = index;

  // destroy prev example before creating the next
  mCurrentExampleView = nullptr;
  mCurrentExampleView = mExamples[mCurrentExampleIndex].factory(engine);

  add_child_bottom(mCurrentExampleView);
}
