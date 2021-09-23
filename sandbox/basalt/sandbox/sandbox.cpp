#include <basalt/sandbox/sandbox.h>

#include <basalt/sandbox/d3d9/device.h>
#include <basalt/sandbox/d3d9/lights.h>
#include <basalt/sandbox/d3d9/matrices.h>
#include <basalt/sandbox/d3d9/meshes.h>
#include <basalt/sandbox/d3d9/textures.h>
#include <basalt/sandbox/d3d9/textures_tci.h>
#include <basalt/sandbox/d3d9/vertices.h>

#include <basalt/sandbox/samples/textures.h>

#include <basalt/sandbox/tribase/02-03_dreieck.h>
#include <basalt/sandbox/tribase/02-04_textures.h>

#include <basalt/api/engine.h>
#include <basalt/api/input_events.h>
#include <basalt/api/prelude.h>
#include <basalt/api/types.h>

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
using basalt::InputEventType;
using basalt::Key;
using basalt::KeyDown;
using basalt::KeyUp;
using basalt::View;
using basalt::WindowMode;

auto ClientApp::create(Engine& engine) -> unique_ptr<ClientApp> {
  return std::make_unique<SandboxApp>(engine);
}

SandboxApp::SandboxApp(Engine& engine)
  : mSandboxView {std::make_shared<SandboxView>(engine)} {
  engine.add_view_top(mSandboxView);
}

void SandboxApp::on_update(Engine&) {
}

SandboxView::SandboxView(Engine& engine) {
  mScenes.reserve(10u);
  mScenes.emplace_back(std::make_shared<d3d9::Device>());
  mScenes.emplace_back(std::make_shared<d3d9::Vertices>(engine));
  mScenes.emplace_back(std::make_shared<d3d9::Matrices>(engine));
  mScenes.emplace_back(std::make_shared<d3d9::Lights>(engine));
  mScenes.emplace_back(std::make_shared<d3d9::Textures>(engine));
  mScenes.emplace_back(std::make_shared<d3d9::TexturesTci>(engine));
  mScenes.emplace_back(std::make_shared<d3d9::Meshes>(engine));

  mScenes.emplace_back(std::make_shared<tribase::Dreieck>(engine));
  mScenes.emplace_back(std::make_shared<tribase::Textures>(engine));

  mScenes.emplace_back(std::make_shared<samples::Textures>(engine));

  engine.set_window_surface_content(mScenes[mCurrentSceneIndex]->drawable());
  engine.add_view_bottom(mScenes[mCurrentSceneIndex]);
}

void SandboxView::tick(Engine& engine) {
  static auto pageUpPressed = false;
  static auto pageDownPressed = false;
  if (is_key_down(Key::PageUp)) {
    if (!pageUpPressed) {
      pageUpPressed = true;

      prev_scene(engine);
    }
  } else {
    pageUpPressed = false;
  }

  if (is_key_down(Key::PageDown)) {
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
}

auto SandboxView::do_handle_input(const InputEvent& event)
  -> InputEventHandled {
  switch (event.type) {
  case InputEventType::KeyDown: {
    switch (event.as<KeyDown>().key) {
    case Key::PageDown:
    case Key::PageUp:
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

void SandboxView::next_scene(Engine& engine) noexcept {
  engine.remove_view(mScenes[mCurrentSceneIndex]);

  mCurrentSceneIndex++;
  if (mCurrentSceneIndex >= mScenes.size()) {
    mCurrentSceneIndex = 0;
  }

  engine.set_window_surface_content(mScenes[mCurrentSceneIndex]->drawable());
  engine.add_view_bottom(mScenes[mCurrentSceneIndex]);
}

void SandboxView::prev_scene(Engine& engine) noexcept {
  engine.remove_view(mScenes[mCurrentSceneIndex]);

  if (mCurrentSceneIndex == 0) {
    mCurrentSceneIndex = mScenes.size() - 1;
  } else {
    mCurrentSceneIndex--;
  }

  engine.set_window_surface_content(mScenes[mCurrentSceneIndex]->drawable());
  engine.add_view_bottom(mScenes[mCurrentSceneIndex]);
}

void SandboxView::set_scene(uSize index, Engine& engine) noexcept {
  BASALT_ASSERT(index < mScenes.size());

  engine.remove_view(mScenes[mCurrentSceneIndex]);

  mCurrentSceneIndex = index;

  engine.set_window_surface_content(mScenes[mCurrentSceneIndex]->drawable());
  engine.add_view_bottom(mScenes[mCurrentSceneIndex]);
}
