#include "runtime/Engine.h"

#include "runtime/dear_imgui.h"
#include "runtime/IApplication.h"

#include "runtime/gfx/Gfx.h"

#include "runtime/gfx/backend/context.h"
#include "runtime/gfx/backend/IRenderer.h"

#include "runtime/platform/Platform.h"

#include "runtime/platform/events/Event.h"
#include "runtime/platform/events/WindowEvents.h"

#include "runtime/shared/Asserts.h"

#include <chrono>
#include <utility>

using std::shared_ptr;
using std::unique_ptr;

namespace basalt {

using gfx::backend::IGfxContext;
using gfx::backend::IRenderer;
using platform::EventType;
using platform::WindowResizedEvent;

namespace {

f64 sCurrentDeltaTime {0.0};
shared_ptr<Scene> sCurrentScene {};
unique_ptr<IRenderer> sRenderer {};
bool sRunning {true};

void update(IApplication* app, IGfxContext* ctx);

void dispatch_pending_events() {
  const auto events = platform::poll_events();
  for (const auto& event : events) {
    switch (event->mType) {
    case EventType::Quit:
    case EventType::WindowCloseRequest:
      sRunning = false;
      break;

    case EventType::WindowResized: {
      const auto resizedEvent = std::static_pointer_cast<WindowResizedEvent>(event);
      sRenderer->on_window_resize(*resizedEvent);
      break;
    }

    default:
      break;
    }
  }
}

} // namespace

void init(unique_ptr<IRenderer> renderer) {
  sRenderer = std::move(renderer);
}

void shutdown() {
  sRenderer.reset();
}

void run(IApplication* app, IGfxContext* const ctx) {
  BASALT_ASSERT_MSG(sCurrentScene, "no scene set");

  static_assert(std::chrono::high_resolution_clock::is_steady);
  using Clock = std::chrono::high_resolution_clock;

  auto startTime = Clock::now();
  do {
    DearImGui::new_frame();

    update(app, ctx);

    const auto endTime = Clock::now();
    sCurrentDeltaTime = static_cast<f64>((endTime - startTime).count()) /
      (Clock::period::den * Clock::period::num);
    startTime = endTime;

    dispatch_pending_events();
  } while (sRunning);
}

void quit() {
  sRunning = false;
}

auto get_delta_time() -> f64 {
  return sCurrentDeltaTime;
}

void set_current_scene(shared_ptr<Scene> scene) {
  sCurrentScene = std::move(scene);
}

auto get_renderer() -> IRenderer* {
  return sRenderer.get();
}

namespace {

void update(IApplication* app, IGfxContext* const ctx) {
  app->on_update();

  // also calls ImGui::Render()
  gfx::render(sRenderer.get(), sCurrentScene);

  ctx->present();
}

} // namespace

} // namespace basalt
