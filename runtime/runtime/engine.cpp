#include "Engine.h"

namespace basalt {

using gfx::backend::IRenderer;

auto Engine::config() const -> const Config& {
  return mConfig;
}

auto Engine::mouse_cursor() const -> MouseCursor {
  return mMouseCursor;
}

void Engine::set_mouse_cursor(const MouseCursor mouseCursor) {
  mMouseCursor = mouseCursor;
  mIsDirty = true;
}

Engine::Engine(Config& config, IRenderer* renderer)
  : renderer {renderer}, mConfig {config} {
}

} // namespace basalt
