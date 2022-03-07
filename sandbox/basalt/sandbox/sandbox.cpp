#include <basalt/sandbox/sandbox.h>

#include <basalt/sandbox/test_case.h>

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

#include <basalt/api/client_app.h>
#include <basalt/api/engine.h>
#include <basalt/api/input_events.h>
#include <basalt/api/prelude.h>
#include <basalt/api/types.h>

#include <basalt/api/shared/asserts.h>
#include <basalt/api/shared/config.h>
#include <basalt/api/shared/types.h>

#include <imgui/imgui.h>

#include <memory>
#include <string>

using std::shared_ptr;
using std::string;
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
using basalt::WindowMode;

void ClientApp::bootstrap(Engine& engine) {
  engine.add_view_top(std::make_shared<SandboxView>(engine));
}

struct SandboxView::Example final {
  shared_ptr<TestCase> view;
  string name;
};

SandboxView::SandboxView(Engine& engine) {
  mExamples.reserve(10u);
  mExamples.emplace_back(Example {
    std::make_shared<d3d9::Device>(),
    "Tutorial 1: Creating a Device"s,
  });
  mExamples.emplace_back(Example {
    std::make_shared<d3d9::Vertices>(engine),
    "Tutorial 2: Rendering Vertices"s,
  });
  mExamples.emplace_back(Example {
    std::make_shared<d3d9::Matrices>(engine),
    "Tutorial 3: Using Matrices"s,
  });
  mExamples.emplace_back(Example {
    std::make_shared<d3d9::Lights>(engine),
    "Tutorial 4: Creating and Using Lights"s,
  });
  mExamples.emplace_back(Example {
    std::make_shared<d3d9::Textures>(engine),
    "Tutorial 5: Using Texture Maps"s,
  });
  mExamples.emplace_back(Example {
    std::make_shared<d3d9::TexturesTci>(engine),
    "Tutorial 5: Using Texture Maps (TCI)"s,
  });
  mExamples.emplace_back(Example {
    std::make_shared<d3d9::Meshes>(engine),
    "Tutorial 6: Using Meshes"s,
  });
  mExamples.emplace_back(Example {
    std::make_shared<tribase::Dreieck>(engine),
    "Bsp. 02-03: Das erste Dreieck"s,
  });
  mExamples.emplace_back(Example {
    std::make_shared<tribase::Textures>(engine),
    "Bsp. 02-03: Texturen"s,
  });
  mExamples.emplace_back(Example {
    std::make_shared<samples::Textures>(engine),
    "Textures"s,
  });

  engine.set_window_surface_content(
    mExamples[mCurrentExampleIndex].view->drawable());
  engine.add_view_bottom(mExamples[mCurrentExampleIndex].view);
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
      for (uSize i {0}; i < mExamples.size(); i++) {
        const bool isCurrent {mCurrentExampleIndex == i};
        if (ImGui::MenuItem(mExamples[i].name.data(), nullptr, isCurrent,
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
  engine.remove_view(mExamples[mCurrentExampleIndex].view);

  mCurrentExampleIndex++;
  if (mCurrentExampleIndex >= mExamples.size()) {
    mCurrentExampleIndex = 0;
  }

  engine.set_window_surface_content(
    mExamples[mCurrentExampleIndex].view->drawable());
  engine.add_view_bottom(mExamples[mCurrentExampleIndex].view);
}

void SandboxView::prev_scene(Engine& engine) noexcept {
  engine.remove_view(mExamples[mCurrentExampleIndex].view);

  if (mCurrentExampleIndex == 0) {
    mCurrentExampleIndex = mExamples.size() - 1;
  } else {
    mCurrentExampleIndex--;
  }

  engine.set_window_surface_content(
    mExamples[mCurrentExampleIndex].view->drawable());
  engine.add_view_bottom(mExamples[mCurrentExampleIndex].view);
}

void SandboxView::set_scene(uSize index, Engine& engine) noexcept {
  BASALT_ASSERT(index < mExamples.size());

  engine.remove_view(mExamples[mCurrentExampleIndex].view);

  mCurrentExampleIndex = index;

  engine.set_window_surface_content(
    mExamples[mCurrentExampleIndex].view->drawable());
  engine.add_view_bottom(mExamples[mCurrentExampleIndex].view);
}
