#include "sandbox.h"

#include "d3d9/device.h"
#include "d3d9/lights.h"
#include "d3d9/matrices.h"
#include "d3d9/meshes.h"
#include "d3d9/textures.h"
#include "d3d9/textures_tci.h"
#include "d3d9/vertices.h"

#include <runtime/input.h>
#include <runtime/prelude.h>

#include <runtime/gfx/backend/context.h>
#include <runtime/platform/platform.h>
#include <runtime/shared/config.h>

#include <imgui/imgui.h>

#include <string>

using std::unique_ptr;
using namespace std::literals;

using basalt::Config;
using basalt::ClientApp;
using basalt::Engine;
using basalt::UpdateContext;
using basalt::WindowMode;
using basalt::Key;
using basalt::gfx::Device;

auto ClientApp::configure() -> Config {
  Config config {Config::defaults()};
  config.appName = "Sandbox"s;
  config.debugUiEnabled = true;

  return config;
}

auto ClientApp::create(Engine& engine) -> unique_ptr<ClientApp> {
  return std::make_unique<SandboxApp>(engine);
}


SandboxApp::SandboxApp(Engine& engine) {
  Device& device = engine.gfx_context().device();

  mScenes.reserve(7u);
  mScenes.push_back(std::make_unique<d3d9::Device>());
  mScenes.push_back(std::make_unique<d3d9::Vertices>(device));
  mScenes.push_back(std::make_unique<d3d9::Matrices>(device));
  mScenes.push_back(std::make_unique<d3d9::Lights>(device));
  mScenes.push_back(std::make_unique<d3d9::Textures>(device));
  mScenes.push_back(
    std::make_unique<d3d9::TexturesTci>(
      device, engine.gfx_context().surface_size()));
  mScenes.push_back(std::make_unique<d3d9::Meshes>(device));
}

void SandboxApp::on_update(const UpdateContext& ctx) {
  static auto pageUpPressed = false;
  static auto pageDownPressed = false;
  if (ctx.input.is_key_down(Key::PageUp)) {
    if (!pageUpPressed) {
      pageUpPressed = true;

      prev_scene();
    }
  } else {
    pageUpPressed = false;
  }

  if (ctx.input.is_key_down(Key::PageDown)) {
    if (!pageDownPressed) {
      pageDownPressed = true;

      next_scene();
    }
  } else {
    pageDownPressed = false;
  }

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      for (uSize i = 0; i < mScenes.size(); i++) {
        const bool isCurrent = mCurrentSceneIndex == i;
        if (ImGui::MenuItem(
          mScenes[i]->name().data(), nullptr, isCurrent, !isCurrent)) {
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

  mScenes[mCurrentSceneIndex]->on_update(ctx);
}

void SandboxApp::next_scene() {
  mCurrentSceneIndex++;
  if (mCurrentSceneIndex >= mScenes.size()) {
    mCurrentSceneIndex = 0;
  }
}

void SandboxApp::prev_scene() {
  if (mCurrentSceneIndex == 0) {
    mCurrentSceneIndex = mScenes.size() - 1;
  } else {
    mCurrentSceneIndex--;
  }
}

void SandboxApp::set_scene(const uSize index) {
  mCurrentSceneIndex = index;
}
