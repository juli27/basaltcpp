#include "resource_cache.h"

#include "backend/device.h"

#include "backend/ext/x_model_support.h"

#include "api/resources/resource_registry.h"

#include <utility>

using namespace std::literals;

namespace basalt::gfx {

ResourceCache::ResourceCache(ResourceRegistryPtr resourceRegistry,
                             DevicePtr device)
  : mResourceRegistry(std::move(resourceRegistry)), mDevice(std::move(device)) {
}

void ResourceCache::load(const GfxModel model) const {
  auto& registry = mResourceRegistry->get<GfxModel>();

  const auto modelExt = std::static_pointer_cast<ext::XModelSupport>(
    mDevice->query_extension("ext_x_model_support"sv).value());

  auto& location = registry.get<FileLocation>(model);
  registry.emplace<ext::ModelHandle>(model, modelExt->load(location.path));
}

void ResourceCache::load(const Texture texture) const {
  auto& registry = mResourceRegistry->get<Texture>();
  auto& location = registry.get<FileLocation>(texture);
  registry.emplace<TextureHandle>(texture, mDevice->add_texture(location.path));
}

auto ResourceCache::get(const GfxModel model) const -> ext::ModelHandle {
  auto& registry = mResourceRegistry->get<GfxModel>();

  return registry.get<ext::ModelHandle>(model);
}

auto ResourceCache::get(const Texture texture) const -> TextureHandle {
  auto& registry = mResourceRegistry->get<Texture>();

  if (!registry.valid(texture)) {
    return TextureHandle::invalid();
  }

  return registry.get<TextureHandle>(texture);
}

} // namespace basalt::gfx
