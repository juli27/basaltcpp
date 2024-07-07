#include <basalt/api/engine.h>

#include <basalt/api/gfx/context.h>

#include <basalt/api/shared/config.h>

#include <utility>

using namespace std::string_literals;

namespace basalt {

auto Engine::config() const noexcept -> Config const& {
  return mConfig;
}

auto Engine::config() noexcept -> Config& {
  return mConfig;
}

auto Engine::gfx_context() const noexcept -> gfx::Context& {
  return *mGfxContext;
}

auto Engine::gfx_info() const noexcept -> gfx::Info const& {
  return mGfxContext->gfx_info();
}

auto Engine::create_gfx_resource_cache() const -> gfx::ResourceCachePtr {
  return mGfxContext->create_resource_cache();
}

auto Engine::root() const -> ViewPtr const& {
  return mRoot;
}

auto Engine::set_root(ViewPtr view) -> void {
  mRoot = std::move(view);
}

auto Engine::mouse_cursor() const noexcept -> MouseCursor {
  return mMouseCursor;
}

auto Engine::set_mouse_cursor(MouseCursor const mouseCursor) noexcept -> void {
  mMouseCursor = mouseCursor;
  mIsDirty = true;
}

auto Engine::set_window_mode(WindowMode const newCanvasMode) noexcept -> void {
  mConfig.set_enum("window.mode"s, newCanvasMode);
}

Engine::Engine(Config config, gfx::ContextPtr gfxContext) noexcept
  : mGfxContext{std::move(gfxContext)}
  , mConfig{std::move(config)} {
}

} // namespace basalt
