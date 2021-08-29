#include <basalt/api/engine.h>

#include <basalt/api/gfx/backend/context.h>

#include <basalt/api/shared/config.h>

#include <utility>

using namespace std::string_literals;

using std::shared_ptr;

namespace basalt {

auto Engine::config() const noexcept -> const Config& {
  return mConfig;
}

auto Engine::resource_registry() const noexcept -> ResourceRegistry& {
  return *mResourceRegistry;
}

auto Engine::gfx_resource_cache() noexcept -> gfx::ResourceCache& {
  return mGfxResourceCache;
}

auto Engine::delta_time() const noexcept -> f64 {
  return mDeltaTime;
}

auto Engine::window_surface_size() const -> Size2Du16 {
  return mGfxContext.surface_size();
}

void Engine::set_window_surface_content(gfx::DrawablePtr drawable) {
  mWindowSurfaceContent = std::move(drawable);
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
  mConfig.set_enum("window.mode"s, windowMode);
  mIsDirty = true;
}

Engine::Engine(Config& config, gfx::Context& context) noexcept
  : mConfig {config}
  , mGfxContext {context}
  , mGfxResourceCache {mResourceRegistry, mGfxContext.device()} {
}

template <>
auto Engine::get_or_load(const Resource resource) -> gfx::ext::XModel {
  if (!mResourceRegistry->has_resource(resource)) {
    mResourceRegistry->register_resource(resource);
  }

  if (mGfxResourceCache.is_loaded(resource)) {
    return mGfxResourceCache.get<gfx::ext::XModel>(resource);
  }

  return mGfxResourceCache.load<gfx::ext::XModel>(resource);
}

template <>
auto Engine::get_or_load(const Resource resource) -> gfx::Texture {
  if (!mResourceRegistry->has_resource(resource)) {
    mResourceRegistry->register_resource(resource);
  }

  if (mGfxResourceCache.is_loaded(resource)) {
    return mGfxResourceCache.get<gfx::Texture>(resource);
  }

  return mGfxResourceCache.load<gfx::Texture>(resource);
}

} // namespace basalt
