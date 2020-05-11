#include "runtime/Engine.h"

#include "runtime/dear_imgui.h"
#include "runtime/IApplication.h"

#include "runtime/gfx/Gfx.h"

#include "runtime/gfx/backend/context.h"
#include "runtime/gfx/backend/IRenderer.h"

#include "runtime/platform/Platform.h"

#include "runtime/platform/events/Event.h"
#include "runtime/platform/events/WindowEvents.h"

#include <chrono>

using std::shared_ptr;

namespace basalt {

using gfx::backend::IGfxContext;
using gfx::backend::IRenderer;
using platform::EventType;
using platform::WindowResizedEvent;

namespace {

f64 sCurrentDeltaTime {0.0};
bool sRunning {true};

void update(IApplication* app, IGfxContext* ctx, IRenderer* renderer, Scene* scene);

void dispatch_pending_events(IRenderer* const renderer) {
  const auto events = platform::poll_events();
  for (const auto& event : events) {
    switch (event->mType) {
    case EventType::Quit:
    case EventType::WindowCloseRequest:
      sRunning = false;
      break;

    case EventType::WindowResized: {
      const auto resizedEvent = std::static_pointer_cast<WindowResizedEvent>(event);
      renderer->on_window_resize(*resizedEvent);
      break;
    }

    default:
      break;
    }
  }
}

} // namespace

void run(IApplication* app, IGfxContext* const ctx, IRenderer* const renderer) {
  static_assert(std::chrono::high_resolution_clock::is_steady);
  using Clock = std::chrono::high_resolution_clock;

  auto startTime = Clock::now();
  do {
    DearImGui::new_frame(renderer);

    update(app, ctx, renderer, get_current_scene());

    const auto endTime = Clock::now();
    sCurrentDeltaTime = static_cast<f64>((endTime - startTime).count()) /
      (Clock::period::den * Clock::period::num);
    startTime = endTime;

    dispatch_pending_events(renderer);
  } while (sRunning);
}

void quit() {
  sRunning = false;
}

auto get_delta_time() -> f64 {
  return sCurrentDeltaTime;
}

namespace {

void update(
  IApplication* app, IGfxContext* const ctx, IRenderer* const renderer
, Scene* scene
) {
  app->on_update();

  // also calls ImGui::Render()
  gfx::render(renderer, scene);

  ctx->present();
}

} // namespace

} // namespace basalt
