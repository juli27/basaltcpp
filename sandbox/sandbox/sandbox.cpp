#include "sandbox/sandbox.h"

#include "sandbox/d3d9/device.h"
#include "sandbox/d3d9/lights.h"
#include "sandbox/d3d9/matrices.h"
#include "sandbox/d3d9/meshes.h"
#include "sandbox/d3d9/textures.h"
#include "sandbox/d3d9/textures_tci.h"
#include "sandbox/d3d9/vertices.h"

#include <runtime/Engine.h>
#include <runtime/Input.h>
#include <runtime/Prelude.h>

#include <runtime/gfx/backend/IRenderer.h>
#include <runtime/platform/Platform.h>

#include <runtime/shared/Config.h>

#include <imgui/imgui.h>

#include <string>

using std::unique_ptr;
using namespace std::literals;

using basalt::Config;
using basalt::ClientApp;
using basalt::WindowMode;
using basalt::gfx::backend::IRenderer;
using basalt::input::Key;

auto ClientApp::configure() -> Config {
  auto config {Config::defaults()};
  config.appName = "Sandbox"s;

  return config;
}


auto ClientApp::create(IRenderer* const renderer) -> unique_ptr<ClientApp> {
  return std::make_unique<SandboxApp>(renderer);
}

SandboxApp::SandboxApp(IRenderer* const renderer) {
  mScenes.reserve(7u);
  mScenes.push_back(std::make_unique<d3d9::Device>());
  mScenes.push_back(std::make_unique<d3d9::Vertices>(renderer));
  mScenes.push_back(std::make_unique<d3d9::Matrices>(renderer));
  mScenes.push_back(std::make_unique<d3d9::Lights>(renderer));
  mScenes.push_back(std::make_unique<d3d9::Textures>(renderer));
  mScenes.push_back(std::make_unique<d3d9::TexturesTci>(renderer));
  mScenes.push_back(std::make_unique<d3d9::Meshes>(renderer));

  mScenes[mCurrentSceneIndex]->on_show();
}

SandboxApp::~SandboxApp() {
  mScenes[mCurrentSceneIndex]->on_hide();
}

void SandboxApp::on_update(const f64 deltaTime) {
  static auto pageUpPressed = false;
  static auto pageDownPressed = false;
  if (basalt::input::is_key_pressed(Key::PageUp)) {
    if (!pageUpPressed) {
      pageUpPressed = true;

      prev_scene();
    }
  } else {
    pageUpPressed = false;
  }

  if (basalt::input::is_key_pressed(Key::PageDown)) {
    if (!pageDownPressed) {
      pageDownPressed = true;

      next_scene();
    }
  } else {
    pageDownPressed = false;
  }

  mScenes[mCurrentSceneIndex]->on_update(deltaTime);

  static bool showSceneDebugUi = false;
  if (showSceneDebugUi) {
    basalt::draw_scene_debug_ui(&showSceneDebugUi);
  }

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
      for (uSize i = 0; i < mScenes.size(); i++) {
        const bool isCurrent = mCurrentSceneIndex == i;
        if (ImGui::MenuItem(mScenes[i]->name().data(), nullptr, isCurrent, !isCurrent)) {
          set_scene(i);
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
      ImGui::MenuItem("Scene Debug UI", nullptr, &showSceneDebugUi);

      ImGui::Separator();

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
      ImGui::MenuItem("Dear ImGui Demo", nullptr, &showDemo);
      ImGui::MenuItem("Dear ImGui Metrics", nullptr, &showMetrics);
      ImGui::MenuItem("About Dear ImGui", nullptr, &showAbout);

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }

  const auto distanceToEdge = 10.0f;
  static auto corner = 2;
  auto& io = ImGui::GetIO();
  const ImVec2 windowPos {(corner & 1) ? io.DisplaySize.x - distanceToEdge : distanceToEdge, (corner & 2) ? io.DisplaySize.y - distanceToEdge : distanceToEdge};
  const ImVec2 windowPosPivot {(corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f};

  ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPosPivot);

  ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
  if (ImGui::Begin(
    "Overlay", nullptr
  , ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration |
    ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
    ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav)) {
    ImGui::Text(
      "%.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

    if (ImGui::BeginPopupContextWindow()) {
      if (ImGui::MenuItem("Top-left", nullptr, corner == 0)) {
        corner = 0;
      }
      if (ImGui::MenuItem("Top-right", nullptr, corner == 1)) {
        corner = 1;
      }
      if (ImGui::MenuItem("Bottom-left", nullptr, corner == 2)) {
        corner = 2;
      }
      if (ImGui::MenuItem("Bottom-right", nullptr, corner == 3)) {
        corner = 3;
      }
      ImGui::EndPopup();
    }
  }
  ImGui::End();
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

  if (mCurrentSceneIndex == 0) {
    mCurrentSceneIndex = static_cast<i32>(mScenes.size() - 1);
  } else {
    mCurrentSceneIndex--;
  }

  mScenes[mCurrentSceneIndex]->on_show();
}

void SandboxApp::set_scene(const uSize index) {
  mScenes[mCurrentSceneIndex]->on_hide();
  mCurrentSceneIndex = index;
  mScenes[mCurrentSceneIndex]->on_show();
}
