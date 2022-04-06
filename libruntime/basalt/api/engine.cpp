#include <basalt/api/engine.h>

#include <basalt/api/resource_registry.h>

#include <basalt/api/gfx/context.h>
#include <basalt/api/gfx/resource_cache.h>

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

auto Engine::resource_registry() const noexcept -> ResourceRegistry& {
  return *mResourceRegistry;
}

auto Engine::resource_manager() noexcept -> ResourceManager& {
  return mResourceManager;
}

auto Engine::gfx_resource_cache() noexcept -> gfx::ResourceCache& {
  return *mGfxResourceCache;
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

auto Engine::set_window_mode(WindowMode const windowMode) noexcept -> void {
  mConfig.set_enum("window.mode"s, windowMode);
  mIsDirty = true;
}

Engine::Engine(Config& config, gfx::ContextPtr gfxContext) noexcept
  : mResourceRegistry{std::make_shared<ResourceRegistry>()}
  , mGfxContext{std::move(gfxContext)}
  , mGfxResourceCache{mGfxContext->create_resource_cache()}
  , mResourceManager{mResourceRegistry, mGfxResourceCache}
  , mConfig{config} {

}

} // namespace basalt
