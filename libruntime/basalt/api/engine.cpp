#include <basalt/api/engine.h>

#include <basalt/api/gfx/context.h>
#include <basalt/api/gfx/resource_cache.h>

#include <basalt/api/shared/config.h>
#include <basalt/api/shared/resource_registry.h>

#include <utility>

using namespace std::string_literals;

namespace basalt {

auto Engine::config() const noexcept -> const Config& {
  return mConfig;
}

auto Engine::config() noexcept -> Config& {
  return mConfig;
}

auto Engine::gfx_context() const noexcept -> gfx::Context& {
  return *mGfxContext;
}

auto Engine::gfx_info() const noexcept -> const gfx::Info& {
  return mGfxContext->gfx_info();
}

auto Engine::create_gfx_resource_cache() const -> gfx::ResourceCachePtr {
  return mGfxContext->create_resource_cache();
}

auto Engine::resource_registry() const noexcept -> ResourceRegistry& {
  return *mResourceRegistry;
}

auto Engine::gfx_resource_cache() noexcept -> gfx::ResourceCache& {
  return *mGfxResourceCache;
}

auto Engine::root() const -> const ViewPtr& {
  return mRoot;
}

auto Engine::set_root(ViewPtr view) -> void {
  mRoot = std::move(view);
}

auto Engine::mouse_cursor() const noexcept -> MouseCursor {
  return mMouseCursor;
}

void Engine::set_mouse_cursor(const MouseCursor mouseCursor) noexcept {
  mMouseCursor = mouseCursor;
  mIsDirty = true;
}

void Engine::set_window_mode(const WindowMode windowMode) noexcept {
  mConfig.set_enum("window.mode"s, windowMode);
  mIsDirty = true;
}

Engine::Engine(Config& config, gfx::ContextPtr gfxContext) noexcept
  : mResourceRegistry {std::make_shared<ResourceRegistry>()}
  , mGfxContext {std::move(gfxContext)}
  , mGfxResourceCache {mGfxContext->create_resource_cache()}
  , mConfig {config} {
}

auto Engine::is_loaded(const ResourceId id) const -> bool {
  return mTextures.find(id) != mTextures.end() ||
         mXModels.find(id) != mXModels.end();
}

template <>
auto Engine::get_or_load(const Resource resource) -> gfx::ext::XModel {
  if (!mResourceRegistry->has_resource(resource)) {
    mResourceRegistry->register_resource(resource);
  }

  if (is_loaded(resource)) {
    return mXModels[resource];
  }

  const auto& path {mResourceRegistry->get_path(resource)};

  return mXModels[resource] = mGfxResourceCache->load_x_model(path);
}

template <>
auto Engine::get_or_load(const Resource resource) -> gfx::Texture {
  if (!mResourceRegistry->has_resource(resource)) {
    mResourceRegistry->register_resource(resource);
  }

  if (is_loaded(resource)) {
    return mTextures[resource];
  }

  const auto& path {mResourceRegistry->get_path(resource)};

  return mTextures[resource] = mGfxResourceCache->load_texture(path);
}

} // namespace basalt
