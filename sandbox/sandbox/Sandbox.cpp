#include "sandbox/Sandbox.h"

#include "sandbox/d3d9_tutorials/Device.h"
#include "sandbox/d3d9_tutorials/Lights.h"
#include "sandbox/d3d9_tutorials/Matrices.h"
#include "sandbox/d3d9_tutorials/Textures.h"
#include "sandbox/d3d9_tutorials/Vertices.h"

#include <runtime/Engine.h>
#include <runtime/Input.h>
#include <runtime/Prelude.h>

#include <runtime/platform/Platform.h>

#include <runtime/shared/Asserts.h>
#include <runtime/shared/Config.h>

#include <imgui/imgui.h>

using std::unique_ptr;

using basalt::Config;
using basalt::IApplication;
using basalt::WindowMode;
using basalt::input::Key;

auto IApplication::configure() -> Config {
  return Config::defaults();
}


auto IApplication::create() -> unique_ptr<IApplication> {
  return std::make_unique<SandboxApp>();
}

SandboxApp::SandboxApp() {
  mScenes.reserve(5u);
  mScenes.push_back(std::make_unique<d3d9::Device>());
  mScenes.push_back(std::make_unique<d3d9::Vertices>());
  mScenes.push_back(std::make_unique<d3d9_tuts::Matrices>());
  mScenes.push_back(std::make_unique<d3d9_tuts::Lights>());
  mScenes.push_back(std::make_unique<d3d9_tuts::Textures>());

  mScenes[mCurrentSceneIndex]->on_show();
}

SandboxApp::~SandboxApp() {
  mScenes[mCurrentSceneIndex]->on_hide();
}

void SandboxApp::on_update() {
  static auto pageUpPressed = false;
  static auto pageDownPressed = false;
  if (basalt::input::is_key_pressed(Key::PageUp)) {
    if (!pageUpPressed) {
      pageUpPressed = true;

      next_scene();
    }
  } else {
    pageUpPressed = false;
  }

  if (basalt::input::is_key_pressed(Key::PageDown)) {
    if (!pageDownPressed) {
      pageDownPressed = true;

      prev_scene();
    }
  } else {
    pageDownPressed = false;
  }

  mScenes[mCurrentSceneIndex]->on_update();

  static auto showDemo = false;
  static auto showMetrics = false;
  static auto showAbout = false;

  if (showDemo) {
    ImGui::ShowDemoWindow(&showDemo);
  }
  if (showMetrics) {
    ImGui::ShowMetricsWindow(&showMetrics);
  }
  if (showAbout) {
    ImGui::ShowAboutWindow(&showAbout);
  }

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Exit", "Alt+F4")) {
        basalt::quit();
      }

      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Scene")) {
      if (ImGui::MenuItem("Next", "PgUp")) {
        next_scene();
      }
      if (ImGui::MenuItem("Prev", "PgDn")) {
        prev_scene();
      }

      ImGui::Separator();

      if (ImGui::BeginMenu("Direct3D 9 Tutorials")) {
        if (ImGui::MenuItem("1: Creating a Device")) {
          set_scene(0);
        }
        if (ImGui::MenuItem("2: Rendering Vertices")) {
          set_scene(1);
        }
        if (ImGui::MenuItem("3: Using Matrices")) {
          set_scene(2);
        }
        if (ImGui::MenuItem("4: Creating and Using Lights")) {
          set_scene(3);
        }
        if (ImGui::MenuItem("5: Using Texture Maps")) {
          set_scene(4);
        }

        ImGui::EndMenu();
      }

      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Window")) {
      const auto currentMode = basalt::platform::get_window_mode();
      if (ImGui::MenuItem(
        "Windowed", nullptr, currentMode == WindowMode::Windowed, false
      )) {
        basalt::platform::set_window_mode(WindowMode::Windowed);
      }
      if (ImGui::MenuItem(
        "Fullscreen", nullptr, currentMode == WindowMode::Fullscreen, false
      )) {
        basalt::platform::set_window_mode(WindowMode::Fullscreen);
      }
      if (ImGui::MenuItem(
        "Fullscreen (Exclusive)", nullptr,
        currentMode == WindowMode::FullscreenExclusive, false
      )) {
        basalt::platform::set_window_mode(WindowMode::FullscreenExclusive);
      }

      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Help")) {
      if (ImGui::MenuItem("Dear ImGui Demo")) {
        showDemo = true;
      }
      if (ImGui::MenuItem("Dear ImGui Metrics")) {
        showMetrics = true;
      }
      if (ImGui::MenuItem("About Dear ImGui")) {
        showAbout = true;
      }

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }
}

void SandboxApp::next_scene() {
  mScenes[mCurrentSceneIndex]->on_hide();

  mCurrentSceneIndex++;
  if (mCurrentSceneIndex >= static_cast<i32>(mScenes.size())) {
    mCurrentSceneIndex = 0;
  }

  mScenes[mCurrentSceneIndex]->on_show();
}

void SandboxApp::prev_scene() {
  mScenes[mCurrentSceneIndex]->on_hide();

  mCurrentSceneIndex--;
  if (mCurrentSceneIndex < 0) {
    mCurrentSceneIndex = static_cast<i32>(mScenes.size() - 1);
  }

  mScenes[mCurrentSceneIndex]->on_show();
}

void SandboxApp::set_scene(const i32 index) {
  BASALT_ASSERT(index < static_cast<i32>(mScenes.size()));
  BASALT_ASSERT(index >= 0);

  mScenes[mCurrentSceneIndex]->on_hide();
  mCurrentSceneIndex = index;
  mScenes[mCurrentSceneIndex]->on_show();
}
