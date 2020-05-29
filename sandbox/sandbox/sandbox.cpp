#include "sandbox/sandbox.h"

#include "sandbox/d3d9/device.h"
#include "sandbox/d3d9/lights.h"
#include "sandbox/d3d9/matrices.h"
#include "sandbox/d3d9/meshes.h"
#include "sandbox/d3d9/textures.h"
#include "sandbox/d3d9/textures_tci.h"
#include "sandbox/d3d9/vertices.h"

#include <runtime/Input.h>
#include <runtime/prelude.h>

#include <runtime/platform/Platform.h>

#include <runtime/shared/Config.h>

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

auto ClientApp::configure() -> Config {
  auto config {Config::defaults()};
  config.appName = "Sandbox"s;
  config.debugUiEnabled = true;

  return config;
}

auto ClientApp::create(
  Engine& engine, const Size2Du16 windowSize) -> unique_ptr<ClientApp> {
  return std::make_unique<SandboxApp>(engine, windowSize);
}


SandboxApp::SandboxApp(
  Engine& engine, const basalt::Size2Du16 windowSize) {
  mScenes.reserve(7u);
  mScenes.push_back(std::make_unique<d3d9::Device>());
  mScenes.push_back(std::make_unique<d3d9::Vertices>(engine.renderer));
  mScenes.push_back(std::make_unique<d3d9::Matrices>(engine.renderer));
  mScenes.push_back(std::make_unique<d3d9::Lights>(engine.renderer));
  mScenes.push_back(std::make_unique<d3d9::Textures>(engine.renderer));
  mScenes.push_back(
    std::make_unique<d3d9::TexturesTci>(engine.renderer, windowSize));
  mScenes.push_back(std::make_unique<d3d9::Meshes>(engine.renderer));

  engine.currentView = mScenes[mCurrentSceneIndex]->view(windowSize);
}

void SandboxApp::on_update(const UpdateContext& ctx) {
  static auto pageUpPressed = false;
  static auto pageDownPressed = false;
  if (ctx.input.is_key_down(Key::PageUp)) {
    if (!pageUpPressed) {
      pageUpPressed = true;

      prev_scene(ctx);
    }
  } else {
    pageUpPressed = false;
  }

  if (ctx.input.is_key_down(Key::PageDown)) {
    if (!pageDownPressed) {
      pageDownPressed = true;

      next_scene(ctx);
    }
  } else {
    pageDownPressed = false;
  }

  mScenes[mCurrentSceneIndex]->on_update(ctx.deltaTime);

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      for (uSize i = 0; i < mScenes.size(); i++) {
        const bool isCurrent = mCurrentSceneIndex == i;
        if (ImGui::MenuItem(
          mScenes[i]->name().data(), nullptr, isCurrent, !isCurrent)) {
          set_scene(ctx, i);
        }
      }

      ImGui::Separator();

      if (ImGui::MenuItem("Next Scene", "PgDn")) {
        next_scene(ctx);
      }
      if (ImGui::MenuItem("Prev Scene", "PgUp")) {
        prev_scene(ctx);
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

void SandboxApp::next_scene(const UpdateContext& ctx) {
  mCurrentSceneIndex++;
  if (mCurrentSceneIndex >= mScenes.size()) {
    mCurrentSceneIndex = 0;
  }

  ctx.engine.currentView = mScenes[mCurrentSceneIndex]->view(ctx.windowSize);
}

void SandboxApp::prev_scene(const UpdateContext& ctx) {
  if (mCurrentSceneIndex == 0) {
    mCurrentSceneIndex = mScenes.size() - 1;
  } else {
    mCurrentSceneIndex--;
  }

  ctx.engine.currentView = mScenes[mCurrentSceneIndex]->view(ctx.windowSize);
}

void SandboxApp::set_scene(const UpdateContext& ctx, const uSize index) {
  mCurrentSceneIndex = index;
  ctx.engine.currentView = mScenes[mCurrentSceneIndex]->view(ctx.windowSize);
}
