#include <basalt/api/engine.h>

#include <basalt/api/gfx/backend/context.h>

#include <basalt/api/shared/config.h>

#include <algorithm>
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

void Engine::add_layer_top(LayerPtr layer) {
  mLayers.emplace(mLayers.begin(), std::move(layer));
}

void Engine::add_layer_bottom(LayerPtr layer) {
  mLayers.emplace_back(std::move(layer));
}

void Engine::add_layer_above(LayerPtr layer, const LayerPtr& before) {
  mLayers.emplace(
    std::find(mLayers.begin(), mLayers.end(), before),
    std::move(layer));
}

void Engine::add_layer_below(LayerPtr layer, const LayerPtr& after) {
  auto it {std::find(mLayers.begin(), mLayers.end(), after)};

  if (it != mLayers.end()) {
    ++it;
  }

  mLayers.emplace(it, std::move(layer));
}

void Engine::remove_layer(const LayerPtr& layer) {
  mLayers.erase(
    std::remove(mLayers.begin(), mLayers.end(), layer),
    mLayers.end());
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
