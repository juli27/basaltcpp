#include "engine.h"

#include "gfx/backend/context.h"

#include <utility>

using std::shared_ptr;

namespace basalt {

using gfx::Context;

auto Engine::config() const noexcept -> const Config& {
  return mConfig;
}

auto Engine::gfx_device() const -> gfx::DevicePtr {
  return mGfxContext->device();
}

auto Engine::gfx_context() const noexcept -> Context& {
  return *mGfxContext;
}

auto Engine::mouse_cursor() const noexcept -> MouseCursor {
  return mMouseCursor;
}

void Engine::set_mouse_cursor(const MouseCursor mouseCursor) noexcept {
  mMouseCursor = mouseCursor;
  mIsDirty = true;
}

auto Engine::load(MaterialDescriptor descriptor) const -> Material {
  auto& materialRegistry = mResourceRegistry->get<Material>();
  const Material material = materialRegistry.create();

  materialRegistry.emplace<MaterialDescriptor>(material, descriptor);
  mGfxResourceCache.load(material);

  return material;
}

Engine::Engine(Config& config, shared_ptr<Context> context) noexcept
  : mConfig {config}
  , mGfxContext {std::move(context)}
  , mGfxResourceCache {mResourceRegistry, mGfxContext->device()} {
}

template <>
auto Engine::load(const std::string_view filePath) const -> gfx::ext::XModel {
  const auto model = mResourceRegistry->add<gfx::ext::XModel>(filePath);
  mGfxResourceCache.load(model);

  return model;
}

template <>
auto Engine::load(const std::string_view filePath) const -> Texture {
  const auto texture = mResourceRegistry->add<Texture>(filePath);
  mGfxResourceCache.load(texture);

  return texture;
}

} // namespace basalt
