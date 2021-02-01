#include "sandbox.h"

#include "d3d9/device.h"
#include "d3d9/lights.h"
#include "d3d9/matrices.h"
#include "d3d9/meshes.h"
#include "d3d9/textures.h"
#include "d3d9/textures_tci.h"
#include "d3d9/vertices.h"

#include <api/engine.h>
#include <api/input_layer.h>
#include <api/prelude.h>

#include <api/shared/config.h>

#include <imgui/imgui.h>

using std::unique_ptr;
using namespace std::literals;

using basalt::ClientApp;
using basalt::Config;
using basalt::Engine;
using basalt::InputEvent;
using basalt::InputEventHandled;
using basalt::InputLayer;
using basalt::Key;
using basalt::UpdateContext;
using basalt::WindowMode;
using basalt::gfx::Device;

struct SandboxApp::Input final : InputLayer {
private:
  auto do_handle_input(const InputEvent&) -> InputEventHandled override {
    return InputEventHandled::Yes;
  }
};

auto ClientApp::configure() -> Config {
  auto config = Config::defaults();
  config.appName = "Sandbox"s;
  config.debugUiEnabled = true;

  return config;
}

auto ClientApp::create(Engine& engine) -> unique_ptr<ClientApp> {
  return std::make_unique<SandboxApp>(engine);
}

SandboxApp::SandboxApp(Engine& engine) : mInput {std::make_shared<Input>()} {
  engine.push_input_layer(mInput);

  mScenes.reserve(7u);
  mScenes.emplace_back(std::make_unique<d3d9::Device>());
  mScenes.emplace_back(std::make_unique<d3d9::Vertices>(engine));
  mScenes.emplace_back(std::make_unique<d3d9::Matrices>(engine));
  mScenes.emplace_back(std::make_unique<d3d9::Lights>(engine));
  mScenes.emplace_back(std::make_unique<d3d9::Textures>(engine));
  mScenes.emplace_back(std::make_unique<d3d9::TexturesTci>(engine));
  mScenes.emplace_back(std::make_unique<d3d9::Meshes>(engine));
}

void SandboxApp::on_update(const UpdateContext& ctx) {
  static auto pageUpPressed = false;
  static auto pageDownPressed = false;
  if (mInput->is_key_down(Key::PageUp)) {
    if (!pageUpPressed) {
      pageUpPressed = true;

      prev_scene();
    }
  } else {
    pageUpPressed = false;
  }

  if (mInput->is_key_down(Key::PageDown)) {
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
        if (ImGui::MenuItem(mScenes[i]->name().data(), nullptr, isCurrent,
                            !isCurrent)) {
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
      const auto currentMode = ctx.engine.config().windowMode;
      if (ImGui::MenuItem("Windowed", nullptr,
                          currentMode == WindowMode::Windowed,
                          currentMode != WindowMode::Windowed)) {
        ctx.engine.set_window_mode(WindowMode::Windowed);
      }
      if (ImGui::MenuItem("Fullscreen", nullptr,
                          currentMode == WindowMode::Fullscreen,
                          currentMode != WindowMode::Fullscreen)) {
        ctx.engine.set_window_mode(WindowMode::Fullscreen);
      }
      if (ImGui::MenuItem("Fullscreen (Exclusive)", nullptr,
                          currentMode == WindowMode::FullscreenExclusive,
                          false)) {
        ctx.engine.set_window_mode(WindowMode::FullscreenExclusive);
      }

      if (ctx.engine.config().debugUiEnabled) {
        ImGui::Separator();
      }

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }

  mScenes[mCurrentSceneIndex]->on_update(ctx);
}

void SandboxApp::next_scene() noexcept {
  mCurrentSceneIndex++;
  if (mCurrentSceneIndex >= mScenes.size()) {
    mCurrentSceneIndex = 0;
  }
}

void SandboxApp::prev_scene() noexcept {
  if (mCurrentSceneIndex == 0) {
    mCurrentSceneIndex = mScenes.size() - 1;
  } else {
    mCurrentSceneIndex--;
  }
}

void SandboxApp::set_scene(const uSize index) noexcept {
  mCurrentSceneIndex = index;
}
