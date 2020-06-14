#include "engine.h"

#include <utility>

using std::shared_ptr;

namespace basalt {

using gfx::Context;

auto Engine::config() const -> const Config& {
  return mConfig;
}

auto Engine::gfx_context() const -> Context& {
  return *mGfxContext;
}

auto Engine::mouse_cursor() const -> MouseCursor {
  return mMouseCursor;
}

void Engine::set_mouse_cursor(const MouseCursor mouseCursor) {
  mMouseCursor = mouseCursor;
  mIsDirty = true;
}

Engine::Engine(Config& config, shared_ptr<Context> context)
  : mConfig {config}, mGfxContext {std::move(context)} {
}

} // namespace basalt
