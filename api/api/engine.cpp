#include "engine.h"

#include "gfx/backend/context.h"

#include "shared/config.h"

#include <utility>

using std::shared_ptr;

namespace basalt {

using gfx::Context;

auto Engine::config() const noexcept -> const Config& {
  return mConfig;
}

auto Engine::resource_registry() const noexcept -> ResourceRegistry& {
  return *mResourceRegistry;
}

auto Engine::gfx_resource_cache() noexcept -> gfx::ResourceCache& {
  return mGfxResourceCache;
}

auto Engine::gfx_device() const -> gfx::DevicePtr {
  return mGfxContext->device();
}

auto Engine::gfx_context() const noexcept -> Context& {
  return *mGfxContext;
}

void Engine::push_input_layer(InputLayerPtr inputTarget) {
  mInputLayers.emplace(mInputLayers.begin(), std::move(inputTarget));
}

auto Engine::mouse_cursor() const noexcept -> MouseCursor {
  return mMouseCursor;
}

void Engine::set_mouse_cursor(const MouseCursor mouseCursor) noexcept {
  mMouseCursor = mouseCursor;
  mIsDirty = true;
}

void Engine::set_window_mode(const WindowMode windowMode) noexcept {
  mConfig.windowMode = windowMode;
  mIsDirty = true;
}

void Engine::load(const Resource resource) {
  get_or_load<gfx::ext::XModel>(resource);
}

Engine::Engine(Config& config, shared_ptr<Context> context) noexcept
  : mConfig {config}
  , mGfxContext {std::move(context)}
  , mGfxResourceCache {mResourceRegistry, mGfxContext->device()} {
}

template <>
auto Engine::get_or_load(const Resource resource) -> gfx::ext::XModel {
  if (!mResourceRegistry->has_resource(resource)) {
    mResourceRegistry->register_resource(resource);
  }

  if (mGfxResourceCache.is_loaded(resource)) {
    return get<gfx::ext::XModel>(resource);
  }

  return mGfxResourceCache.load<gfx::ext::XModel>(resource);
}

template <>
auto Engine::get_or_load(const Resource resource) -> gfx::Texture {
  if (!mResourceRegistry->has_resource(resource)) {
    mResourceRegistry->register_resource(resource);
  }

  if (mGfxResourceCache.is_loaded(resource)) {
    return get<gfx::Texture>(resource);
  }

  return mGfxResourceCache.load<gfx::Texture>(resource);
}

template <>
auto Engine::get(const ResourceId id) -> gfx::ext::XModel {
  return mGfxResourceCache.get<gfx::ext::XModel>(id);
}

template <>
auto Engine::get(const ResourceId id) -> gfx::Texture {
  return mGfxResourceCache.get<gfx::Texture>(id);
}

} // namespace basalt
