#include "Sandbox.h"

#include "d3d9_tutorials/Device.h"
#include "d3d9_tutorials/Lights.h"
#include "d3d9_tutorials/Matrices.h"
#include "d3d9_tutorials/Textures.h"
#include "d3d9_tutorials/Vertices.h"

#include <basalt/Prelude.h> // input, platform, ImGui

#include <memory> // make_unique

using bs::Config;
using bs::WindowMode;
using bs::input::Key;

// TODO: entity update system
// TODO: ImGui updater editor

SandboxApp::SandboxApp(Config& config) {
  config.mWindow.mTitle = "Basalt Sandbox";
  config.mWindow.mMode = WindowMode::Windowed;
  config.mWindow.mResizeable = true;
}

void SandboxApp::next_scene() {
  mScenes.at(mCurrentSceneIndex)->on_hide();
  mCurrentSceneIndex++;
  if (mCurrentSceneIndex >= static_cast<i32>(mScenes.size())) {
    mCurrentSceneIndex = 0;
  }

  mScenes.at(mCurrentSceneIndex)->on_show();
}

void SandboxApp::prev_scene() {
  mScenes.at(mCurrentSceneIndex)->on_hide();
  mCurrentSceneIndex--;
  if (mCurrentSceneIndex < 0) {
    mCurrentSceneIndex = static_cast<i32>(mScenes.size() - 1);
  }

  mScenes.at(mCurrentSceneIndex)->on_show();
}

void SandboxApp::set_scene(const i32 index) {
  BS_ASSERT(index < static_cast<i32>(mScenes.size()), "");
  BS_ASSERT(index >= 0, "");

  mScenes.at(mCurrentSceneIndex)->on_hide();
  mCurrentSceneIndex = index;
  mScenes.at(mCurrentSceneIndex)->on_show();
}

void SandboxApp::on_init() {
  mScenes.push_back(std::make_unique<d3d9_tuts::Device>());
  mScenes.push_back(std::make_unique<d3d9_tuts::Vertices>());
  mScenes.push_back(std::make_unique<d3d9_tuts::Matrices>());
  mScenes.push_back(std::make_unique<d3d9_tuts::Lights>());
  mScenes.push_back(std::make_unique<d3d9_tuts::Textures>());

  mScenes.at(mCurrentSceneIndex)->on_show();
}

void SandboxApp::on_shutdown() {
  mScenes.clear();
}

void SandboxApp::on_update() {
  // HACK
  static auto rightPressed = false;
  static auto leftPressed = false;
  if (bs::input::is_key_pressed(Key::PageUp)) {
    if (!rightPressed) {
      rightPressed = true;

      next_scene();
    }
  } else {
    rightPressed = false;
  }

  if (bs::input::is_key_pressed(Key::PageDown)) {
    if (!leftPressed) {
      leftPressed = true;

      prev_scene();
    }
  } else {
    leftPressed = false;
  }

  mScenes.at(mCurrentSceneIndex)->on_update();

  static auto sShowDemo = false;
  static auto sShowMetrics = false;
  static auto sShowAbout = false;

  if (sShowDemo) {
    ImGui::ShowDemoWindow(&sShowDemo);
  }
  if (sShowMetrics) {
    ImGui::ShowMetricsWindow(&sShowMetrics);
  }
  if (sShowAbout) {
    ImGui::ShowAboutWindow(&sShowAbout);
  }

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Exit", "Alt+F4")) {
        bs::platform::request_quit();
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
      const auto currentMode = bs::platform::get_window_mode();
      if (ImGui::MenuItem(
        "Windowed", nullptr, currentMode == WindowMode::Windowed
      )) {
        bs::platform::set_window_mode(WindowMode::Windowed);
      }
      if (ImGui::MenuItem(
        "Fullscreen", nullptr, currentMode == WindowMode::Fullscreen
      )) {
        bs::platform::set_window_mode(WindowMode::Fullscreen);
      }
      if (ImGui::MenuItem(
        "Fullscreen (Exclusive)", nullptr,
        currentMode == WindowMode::FullscreenExclusive
      )) {
        bs::platform::set_window_mode(WindowMode::FullscreenExclusive);
      }

      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Help")) {
      if (ImGui::MenuItem("Dear ImGui Demo")) {
        sShowDemo = true;
      }
      if (ImGui::MenuItem("Dear ImGui Metrics")) {
        sShowMetrics = true;
      }
      if (ImGui::MenuItem("About Dear ImGui")) {
        sShowAbout = true;
      }

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }
}

auto bs::IApplication::create(Config& config) -> IApplication* {
  return new SandboxApp(config);
}
