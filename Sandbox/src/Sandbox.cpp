#include "Sandbox.h"

#include <memory> // make_unique

#include <BasaltPrelude.h> // input, platform, ImGui

#include "d3d9_tutorials/Device.h"
#include "d3d9_tutorials/Lights.h"
#include "d3d9_tutorials/Matrices.h"
#include "d3d9_tutorials/Textures.h"
#include "d3d9_tutorials/Vertices.h"

using bs::Config;
using bs::WindowMode;
using bs::input::Key;

// TODO: entity update system
// TODO: ImGui updater editor

SandboxApp::SandboxApp(Config& config) {
  config.mWindow.mTitle = "Basalt Sandbox";
  config.mWindow.mMode = WindowMode::Windowed;
}

void SandboxApp::NextScene() {
  mScenes.at(mCurrentSceneIndex)->OnHide();
  mCurrentSceneIndex++;
  if (mCurrentSceneIndex >= static_cast<i32>(mScenes.size())) {
    mCurrentSceneIndex = 0;
  }

  mScenes.at(mCurrentSceneIndex)->OnShow();
}

void SandboxApp::PrevScene() {
  mScenes.at(mCurrentSceneIndex)->OnHide();
  mCurrentSceneIndex--;
  if (mCurrentSceneIndex < 0) {
    mCurrentSceneIndex = static_cast<i32>(mScenes.size() - 1);
  }

  mScenes.at(mCurrentSceneIndex)->OnShow();
}

void SandboxApp::SetScene(const i32 index) {
  BS_RELEASE_ASSERT(index < static_cast<i32>(mScenes.size()), "");
  BS_RELEASE_ASSERT(index >= 0, "");

  mScenes.at(mCurrentSceneIndex)->OnHide();
  mCurrentSceneIndex = index;
  mScenes.at(mCurrentSceneIndex)->OnShow();
}

void SandboxApp::OnInit() {
  mScenes.push_back(std::make_unique<d3d9_tuts::Device>());
  mScenes.push_back(std::make_unique<d3d9_tuts::Vertices>());
  mScenes.push_back(std::make_unique<d3d9_tuts::Matrices>());
  mScenes.push_back(std::make_unique<d3d9_tuts::Lights>());
  mScenes.push_back(std::make_unique<d3d9_tuts::Textures>());

  mScenes.at(mCurrentSceneIndex)->OnShow();
}

void SandboxApp::OnShutdown() {
  mScenes.clear();
}

void SandboxApp::OnUpdate() {
  // HACK
  static auto rightPressed = false;
  static auto leftPressed = false;
  if (bs::input::IsKeyPressed(Key::PageUp)) {
    if (!rightPressed) {
      rightPressed = true;

      NextScene();
    }
  } else {
    rightPressed = false;
  }

  if (bs::input::IsKeyPressed(Key::PageDown)) {
    if (!leftPressed) {
      leftPressed = true;

      PrevScene();
    }
  } else {
    leftPressed = false;
  }

  mScenes.at(mCurrentSceneIndex)->OnUpdate();

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
        bs::platform::RequestQuit();
      }

      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Scene")) {
      if (ImGui::MenuItem("Next", "PgUp")) {
        NextScene();
      }
      if (ImGui::MenuItem("Prev", "PgDn")) {
        PrevScene();
      }

      ImGui::Separator();

      if (ImGui::BeginMenu("Direct3D 9 Tutorials")) {
        if (ImGui::MenuItem("1: Creating a Device")) {
          SetScene(0);
        }
        if (ImGui::MenuItem("2: Rendering Vertices")) {
          SetScene(1);
        }
        if (ImGui::MenuItem("3: Using Matrices")) {
          SetScene(2);
        }
        if (ImGui::MenuItem("4: Creating and Using Lights")) {
          SetScene(3);
        }
        if (ImGui::MenuItem("5: Using Texture Maps")) {
          SetScene(4);
        }

        ImGui::EndMenu();
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

auto bs::IApplication::Create(Config& config) -> IApplication* {
  return new SandboxApp(config);
}
