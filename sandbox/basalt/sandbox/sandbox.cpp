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

#include <basalt/api/client_app.h>
#include <basalt/api/engine.h>
#include <basalt/api/input_events.h>
#include <basalt/api/prelude.h>
#include <basalt/api/types.h>

#include <basalt/api/shared/asserts.h>
#include <basalt/api/shared/config.h>
#include <basalt/api/shared/types.h>

#include <imgui/imgui.h>

#include <memory>
#include <string>

using std::shared_ptr;
using std::string;
using namespace std::literals;

using basalt::ClientApp;
using basalt::Config;
using basalt::Engine;
using basalt::InputEvent;
using basalt::InputEventHandled;
using basalt::InputEventType;
using basalt::Key;
using basalt::KeyDown;
using basalt::KeyUp;
using basalt::ViewPtr;
using basalt::WindowMode;

void ClientApp::bootstrap(Engine& engine) {
  engine.set_root(std::make_shared<SandboxView>(engine));
}

struct SandboxView::Example final {
  ViewPtr view;
  string name;
};

SandboxView::SandboxView(Engine& engine) {
  mExamples.reserve(10u);
  mExamples.emplace_back(Example {
    std::make_shared<d3d9::Device>(),
    "Tutorial 1: Creating a Device"s,
  });
  mExamples.emplace_back(Example {
    std::make_shared<d3d9::Vertices>(engine),
    "Tutorial 2: Rendering Vertices"s,
  });
  mExamples.emplace_back(Example {
    std::make_shared<d3d9::Matrices>(engine),
    "Tutorial 3: Using Matrices"s,
  });
  mExamples.emplace_back(Example {
    std::make_shared<d3d9::Lights>(engine),
    "Tutorial 4: Creating and Using Lights"s,
  });
  mExamples.emplace_back(Example {
    std::make_shared<d3d9::Textures>(engine),
    "Tutorial 5: Using Texture Maps"s,
  });
  mExamples.emplace_back(Example {
    std::make_shared<d3d9::Meshes>(engine),
    "Tutorial 6: Using Meshes"s,
  });
  mExamples.emplace_back(Example {
    std::make_shared<tribase::Dreieck>(engine),
    "Bsp. 02-03: Das erste Dreieck"s,
  });
  mExamples.emplace_back(Example {
    std::make_shared<tribase::Textures>(engine),
    "Bsp. 02-03: Texturen"s,
  });
  mExamples.emplace_back(Example {
    std::make_shared<samples::Textures>(engine),
    "Textures"s,
  });
  mExamples.emplace_back(Example {
    std::make_shared<samples::SimpleScene>(engine),
    "Simple Scene"s,
  });

  add_child_bottom(mExamples[mCurrentExampleIndex].view);
}

void SandboxView::on_tick(Engine& engine) {
  // https://github.com/ocornut/imgui/issues/331
  enum class OpenPopup : u8 { None, GfxInfo };
  OpenPopup shouldOpenPopup {OpenPopup::None};

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      for (uSize i {0}; i < mExamples.size(); i++) {
        const bool isCurrent {mCurrentExampleIndex == i};
        if (ImGui::MenuItem(mExamples[i].name.data(), nullptr, isCurrent,
                            !isCurrent)) {
          switch_scene(i);
        }
      }

      ImGui::Separator();

      if (ImGui::MenuItem("Next Scene", "PgDn")) {
        next_scene();
      }
      if (ImGui::MenuItem("Prev Scene", "PgUp")) {
        prev_scene();
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

  if (mShowDemo) {
    ImGui::ShowDemoWindow(&mShowDemo);
  }
  if (mShowMetrics) {
    ImGui::ShowMetricsWindow(&mShowMetrics);
  }
  if (mShowAbout) {
    ImGui::ShowAboutWindow(&mShowAbout);
  }
}

auto SandboxView::on_input(const InputEvent& event) -> InputEventHandled {
  switch (event.type) {
  case InputEventType::KeyDown: {
    switch (event.as<KeyDown>().key) {
    case Key::PageDown:
      next_scene();

      return InputEventHandled::Yes;

    case Key::PageUp:
      prev_scene();

      return InputEventHandled::Yes;

    default:
      break;
    }

    break;
  }

  case InputEventType::KeyUp: {
    switch (event.as<KeyUp>().key) {
    case Key::PageDown:
    case Key::PageUp:
      return InputEventHandled::Yes;

    default:
      break;
    }

    break;
  }

  default:
    break;
  }

  return InputEventHandled::No;
}

void SandboxView::next_scene() noexcept {
  uSize nextSceneIndex {mCurrentExampleIndex + 1};
  if (nextSceneIndex >= mExamples.size()) {
    nextSceneIndex = 0;
  }

  switch_scene(nextSceneIndex);
}

void SandboxView::prev_scene() noexcept {
  const uSize prevSceneIndex {[this] {
    if (mCurrentExampleIndex == 0) {
      return mExamples.size() - 1;
    }

    return mCurrentExampleIndex - 1;
  }()};

  switch_scene(prevSceneIndex);
}

void SandboxView::switch_scene(const uSize index) noexcept {
  if (mExamples.empty()) {
    return;
  }

  BASALT_ASSERT(index < mExamples.size());

  remove_child(mExamples[mCurrentExampleIndex].view);

  mCurrentExampleIndex = index;

  add_child_bottom(mExamples[mCurrentExampleIndex].view);
}
