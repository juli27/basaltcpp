#include <basalt/sandbox/sandbox.h>

#include <basalt/sandbox/d3d9_tutorials.h>

#include <basalt/sandbox/samples/samples.h>
#include <basalt/sandbox/tribase/tribase_examples.h>

#include <basalt/api/client_app.h>
#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/shared/asserts.h>
#include <basalt/api/shared/config.h>

#include <imgui/imgui.h>

#include <functional>
#include <memory>
#include <string>

using std::function;
using std::string;
using namespace std::literals;

using basalt::ClientApp;
using basalt::Config;
using basalt::Engine;
using basalt::ViewPtr;
using basalt::WindowMode;

auto ClientApp::bootstrap(Engine& engine) -> void {
  engine.set_root(std::make_shared<SandboxView>(engine));
}

struct SandboxView::Example final {
  string name;
  function<ViewPtr(Engine&)> factory;
};

SandboxView::SandboxView(Engine& engine) {
  mExamples.reserve(18u);
  mExamples.emplace_back(Example {
    "Tutorial 1: Creating a Device"s,
    &D3D9Tutorials::new_device_tutorial,
  });
  mExamples.emplace_back(Example {
    "Tutorial 2: Rendering Vertices"s,
    &D3D9Tutorials::new_vertices_tutorial,
  });
  mExamples.emplace_back(Example {
    "Tutorial 3: Using Matrices"s,
    &D3D9Tutorials::new_matrices_tutorial,
  });
  mExamples.emplace_back(Example {
    "Tutorial 4: Creating and Using Lights"s,
    &D3D9Tutorials::new_lights_tutorial,
  });
  mExamples.emplace_back(Example {
    "Tutorial 5: Using Texture Maps"s,
    &D3D9Tutorials::new_textures_tutorial,
  });
  mExamples.emplace_back(Example {
    "Tutorial 6: Using Meshes"s,
    &D3D9Tutorials::new_meshes_tutorial,
  });
  mExamples.emplace_back(Example {
    "Bsp. 02-03: Das erste Dreieck"s,
    &TribaseExamples::new_first_triangle_example,
  });
  mExamples.emplace_back(Example {
    "Bsp. 02-04: Texturen"s,
    &TribaseExamples::new_02_04_textures_example,
  });
  mExamples.emplace_back(Example {
    "Bsp. 02-04: Texturen - Aufgabe 1-3"s,
    &TribaseExamples::new_02_04_textures_exercises,
  });
  mExamples.emplace_back(Example {
    "Bsp. 02-05: Vertex- und Index-Buffer"s,
    &TribaseExamples::new_02_05_buffers_example,
  });
  mExamples.emplace_back(Example {
    "Bsp. 02-05: Vertex- und Index-Buffer - Aufgabe 1-2"s,
    &TribaseExamples::new_02_05_buffers_exercises,
  });
  mExamples.emplace_back(Example {
    "Bsp. 02-06: Nebel"s,
    &TribaseExamples::new_02_06_fog_example,
  });
  mExamples.emplace_back(Example {
    "Bsp. 02-07: Beleuchtung"s,
    &TribaseExamples::new_02_07_lighting_example,
  });
  mExamples.emplace_back(Example {
    "Bsp. 02-08: Alpha Blending"s,
    &TribaseExamples::new_02_08_blending_example,
  });
  mExamples.emplace_back(Example {
    "Textures"s,
    &Samples::new_textures_sample,
  });
  mExamples.emplace_back(Example {
    "Simple Scene"s,
    &Samples::new_simple_scene_sample,
  });
  mExamples.emplace_back(Example {
    "Cubes"s,
    &Samples::new_cubes_sample,
  });
  mExamples.emplace_back(Example {
    "Lighting"s,
    &Samples::new_lighting_sample,
  });

  switch_scene(mCurrentExampleIndex, engine);
}

auto SandboxView::on_update(UpdateContext& ctx) -> void {
  Engine& engine {ctx.engine};
  Config& config {engine.config()};

  // https://github.com/ocornut/imgui/issues/331
  enum class OpenPopup : u8 { None, GfxInfo };
  OpenPopup shouldOpenPopup {OpenPopup::None};

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      for (uSize i {0}; i < mExamples.size(); i++) {
        const bool isCurrent {mCurrentExampleIndex == i};
        if (ImGui::MenuItem(mExamples[i].name.data(), nullptr, isCurrent,
                            !isCurrent)) {
          switch_scene(i, engine);
        }
      }

      ImGui::Separator();

      if (ImGui::MenuItem("Next Scene", "PgDn")) {
        next_scene(engine);
      }
      if (ImGui::MenuItem("Prev Scene", "PgUp")) {
        prev_scene(engine);
      }

      ImGui::Separator();

      if (ImGui::MenuItem("Exit", "Alt+F4")) {
        basalt::quit();
      }

      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View")) {
      const auto currentMode =
        config.get_enum("window.mode"s, basalt::to_window_mode);
      if (ImGui::MenuItem("Windowed", nullptr,
                          currentMode == WindowMode::Windowed,
                          currentMode != WindowMode::Windowed)) {
        engine.set_window_mode(WindowMode::Windowed);
      }
      if (ImGui::MenuItem("Fullscreen", nullptr,
                          currentMode == WindowMode::Fullscreen,
                          currentMode != WindowMode::Fullscreen)) {
        engine.set_window_mode(WindowMode::Fullscreen);
      }
      if (ImGui::MenuItem("Fullscreen (Exclusive)", nullptr,
                          currentMode == WindowMode::FullscreenExclusive,
                          currentMode != WindowMode::FullscreenExclusive)) {
        engine.set_window_mode(WindowMode::FullscreenExclusive);
      }

      ImGui::Separator();

      if (ImGui::MenuItem("GFX Info...")) {
        shouldOpenPopup = OpenPopup::GfxInfo;
      }

      bool sceneInspectorEnabled {
        config.get_bool("debug.scene_inspector.visible"s)};

      ImGui::MenuItem("Scene Inspector", nullptr, &sceneInspectorEnabled);

      config.set_bool("debug.scene_inspector.visible"s, sceneInspectorEnabled);

      ImGui::MenuItem("Performance Overlay", nullptr, &mShowOverlay);

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

  ImGui::SetNextWindowSize(ImVec2 {300.0f, 350.0f}, ImGuiCond_FirstUseEver);
  if (ImGui::BeginPopupModal("Gfx Info", nullptr)) {
    const f32 footerHeight {ImGui::GetFrameHeightWithSpacing()};

    if (ImGui::BeginChild("##info", ImVec2 {0.0f, -footerHeight})) {
      mDebugUi.show_gfx_info(engine.gfx_info());
    }

    ImGui::EndChild();

    if (ImGui::Button("Close", ImVec2 {120.0f, 0.0f})) {
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
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

  if (ImGui::IsKeyPressed(ImGuiKey_PageDown)) {
    next_scene(engine);
  } else if (ImGui::IsKeyPressed(ImGuiKey_PageUp)) {
    prev_scene(engine);
  }
}

auto SandboxView::next_scene(Engine& engine) -> void {
  uSize nextSceneIndex {mCurrentExampleIndex + 1};
  if (nextSceneIndex >= mExamples.size()) {
    nextSceneIndex = 0;
  }

  switch_scene(nextSceneIndex, engine);
}

auto SandboxView::prev_scene(Engine& engine) -> void {
  const uSize prevSceneIndex {[this] {
    if (mCurrentExampleIndex == 0) {
      return mExamples.size() - 1;
    }

    return mCurrentExampleIndex - 1;
  }()};

  switch_scene(prevSceneIndex, engine);
}

auto SandboxView::switch_scene(const uSize index, Engine& engine) -> void {
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
