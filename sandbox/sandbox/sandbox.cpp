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
#include <runtime/prelude.h>

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
  config.debugUiEnabled = true;

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

      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View")) {
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

      ImGui::Separator();

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
