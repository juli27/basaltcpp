#include <basalt/sandbox/sandbox.h>

#include <basalt/sandbox/d3d9/device.h>
#include <basalt/sandbox/d3d9/lights.h>
#include <basalt/sandbox/d3d9/matrices.h>
#include <basalt/sandbox/d3d9/meshes.h>
#include <basalt/sandbox/d3d9/textures.h>
#include <basalt/sandbox/d3d9/textures_tci.h>
#include <basalt/sandbox/d3d9/vertices.h>

#include <basalt/sandbox/tribase/02-03_dreieck.h>

#include <basalt/api/engine.h>
#include <basalt/api/input_layer.h>
#include <basalt/api/prelude.h>

#include <basalt/api/shared/asserts.h>
#include <basalt/api/shared/config.h>
#include <basalt/api/shared/types.h>

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
using basalt::WindowMode;

struct SandboxApp::Input final : InputLayer {
private:
  auto do_handle_input(const InputEvent&) -> InputEventHandled override {
    return InputEventHandled::Yes;
  }
};

auto ClientApp::create(Engine& engine) -> unique_ptr<ClientApp> {
  return std::make_unique<SandboxApp>(engine);
}

SandboxApp::SandboxApp(Engine& engine) : mInput {std::make_shared<Input>()} {
  engine.push_input_layer(mInput);

  mScenes.reserve(8u);
  mScenes.emplace_back(std::make_unique<d3d9::Device>());
  mScenes.emplace_back(std::make_unique<d3d9::Vertices>(engine));
  mScenes.emplace_back(std::make_unique<d3d9::Matrices>(engine));
  mScenes.emplace_back(std::make_unique<d3d9::Lights>(engine));
  mScenes.emplace_back(std::make_unique<d3d9::Textures>(engine));
  mScenes.emplace_back(std::make_unique<d3d9::TexturesTci>(engine));
  mScenes.emplace_back(std::make_unique<d3d9::Meshes>(engine));

  mScenes.emplace_back(std::make_unique<tribase::Dreieck>(engine));

  engine.set_window_surface_content(mScenes[mCurrentSceneIndex]->drawable());
}

void SandboxApp::on_update(Engine& engine) {
  static auto pageUpPressed = false;
  static auto pageDownPressed = false;
  if (mInput->is_key_down(Key::PageUp)) {
    if (!pageUpPressed) {
      pageUpPressed = true;

      prev_scene(engine);
    }
  } else {
    pageUpPressed = false;
  }

  if (mInput->is_key_down(Key::PageDown)) {
    if (!pageDownPressed) {
      pageDownPressed = true;

      next_scene(engine);
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
          set_scene(i, engine);
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
      const Config& config = engine.config();
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

      if (config.get_bool("runtime.debugUI.enabled"s)) {
        ImGui::Separator();
      }

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }

  mScenes[mCurrentSceneIndex]->on_update(engine);
}

void SandboxApp::next_scene(Engine& engine) noexcept {
  mCurrentSceneIndex++;
  if (mCurrentSceneIndex >= mScenes.size()) {
    mCurrentSceneIndex = 0;
  }

  engine.set_window_surface_content(mScenes[mCurrentSceneIndex]->drawable());
}

void SandboxApp::prev_scene(Engine& engine) noexcept {
  if (mCurrentSceneIndex == 0) {
    mCurrentSceneIndex = mScenes.size() - 1;
  } else {
    mCurrentSceneIndex--;
  }

  engine.set_window_surface_content(mScenes[mCurrentSceneIndex]->drawable());
}

void SandboxApp::set_scene(const uSize index, Engine& engine) noexcept {
  BASALT_ASSERT(index < mScenes.size());

  mCurrentSceneIndex = index;
  engine.set_window_surface_content(mScenes[mCurrentSceneIndex]->drawable());
}
