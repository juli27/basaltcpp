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

auto Engine::canvas_pointer() const -> CanvasPointer {
  return mCanvasPointer;
}

auto Engine::set_canvas_pointer(CanvasPointer const mouseCursor) -> void {
  mCanvasPointer = mouseCursor;
  mIsDirty = true;
}

Engine::Engine(Config config, gfx::ContextPtr gfxContext) noexcept
  : mGfxContext{std::move(gfxContext)}
  , mConfig{std::move(config)} {
}

} // namespace basalt
