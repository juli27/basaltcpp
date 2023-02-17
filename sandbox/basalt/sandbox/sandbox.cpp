#include <basalt/sandbox/sandbox.h>

#include <basalt/sandbox/d3d9/device.h>
#include <basalt/sandbox/d3d9/lights.h>
#include <basalt/sandbox/d3d9/matrices.h>
#include <basalt/sandbox/d3d9/meshes.h>
#include <basalt/sandbox/d3d9/textures.h>
#include <basalt/sandbox/d3d9/vertices.h>

#include <basalt/sandbox/samples/simple_scene.h>
#include <basalt/sandbox/samples/textures.h>

#include <basalt/sandbox/tribase/02-03_dreieck.h>
#include <basalt/sandbox/tribase/02-04_textures.h>
#include <basalt/sandbox/tribase/02-04_textures_ex1.h>

#include <basalt/api/client_app.h>
#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>
#include <basalt/api/types.h>

#include <basalt/api/shared/asserts.h>
#include <basalt/api/shared/config.h>
#include <basalt/api/shared/types.h>

#include <basalt/api/base/utils.h>

#include <imgui/imgui.h>

#include <functional>
#include <memory>
#include <string>

using std::function;
using std::shared_ptr;
using std::string;
using namespace std::literals;

using basalt::ClientApp;
using basalt::Config;
using basalt::Engine;
using basalt::Key;
using basalt::ViewPtr;
using basalt::WindowMode;

namespace {

template <typename T>
auto create_example(Engine& engine) -> ViewPtr {
  return std::make_shared<T>(engine);
}

} // namespace

auto ClientApp::bootstrap(Engine& engine) -> void {
  engine.set_root(std::make_shared<SandboxView>(engine));
}

struct SandboxView::Example final {
  string name;
  function<ViewPtr(Engine&)> factory;
};

SandboxView::SandboxView(Engine& engine) {
  mExamples.reserve(11u);
  mExamples.emplace_back(Example {
    "Tutorial 1: Creating a Device"s,
    &create_example<d3d9::Device>,
  });
  mExamples.emplace_back(Example {
    "Tutorial 2: Rendering Vertices"s,
    &create_example<d3d9::Vertices>,
  });
  mExamples.emplace_back(Example {
    "Tutorial 3: Using Matrices"s,
    &create_example<d3d9::Matrices>,
  });
  mExamples.emplace_back(Example {
    "Tutorial 4: Creating and Using Lights"s,
    &create_example<d3d9::Lights>,
  });
  mExamples.emplace_back(Example {
    "Tutorial 5: Using Texture Maps"s,
    &create_example<d3d9::Textures>,
  });
  mExamples.emplace_back(Example {
    "Tutorial 6: Using Meshes"s,
    &create_example<d3d9::Meshes>,
  });
  mExamples.emplace_back(Example {
    "Bsp. 02-03: Das erste Dreieck"s,
    &create_example<tribase::Dreieck>,
  });
  mExamples.emplace_back(Example {
    "Bsp. 02-03: Texturen"s,
    &create_example<tribase::Textures>,
  });
  mExamples.emplace_back(Example {
    "Bsp. 02-03: Texturen - Aufgabe 1"s,
    &create_example<tribase::TexturesEx1>,
  });
  mExamples.emplace_back(Example {
    "Textures"s,
    &create_example<samples::Textures>,
  });
  mExamples.emplace_back(Example {
    "Simple Scene"s,
    &create_example<samples::SimpleScene>,
  });

  switch_scene(mCurrentExampleIndex, engine);
}

auto SandboxView::on_tick(Engine& engine) -> void {
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
      Config& config {engine.config()};
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
        config.get_bool("debug.scene_inspector.enabled"s)};

      ImGui::MenuItem("Scene Inspector", nullptr, &sceneInspectorEnabled);

      config.set_bool("debug.scene_inspector.enabled"s, sceneInspectorEnabled);

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

  if (ImGui::IsKeyPressed(enum_cast(Key::PageDown), false)) {
    next_scene(engine);
  } else if (ImGui::IsKeyPressed(enum_cast(Key::PageUp), false)) {
    prev_scene(engine);
  }
}

auto SandboxView::next_scene(Engine& engine) noexcept -> void {
  uSize nextSceneIndex {mCurrentExampleIndex + 1};
  if (nextSceneIndex >= mExamples.size()) {
    nextSceneIndex = 0;
  }

  switch_scene(nextSceneIndex, engine);
}

auto SandboxView::prev_scene(Engine& engine) noexcept -> void {
  const uSize prevSceneIndex {[this] {
    if (mCurrentExampleIndex == 0) {
      return mExamples.size() - 1;
    }

    return mCurrentExampleIndex - 1;
  }()};

  switch_scene(prevSceneIndex, engine);
}

auto SandboxView::switch_scene(const uSize index, Engine& engine) noexcept
  -> void {
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
