#include <basalt/api/resource_manager.h>

#include <basalt/api/resource_registry.h>

#include <basalt/api/gfx/resource_cache.h>

#include <utility>

namespace basalt {

ResourceManager::ResourceManager(ResourceRegistryPtr registry,
                                 gfx::ResourceCachePtr gfxResources)
  : mResourceRegistry{std::move(registry)}
  , mGfxResources{std::move(gfxResources)} {
}

auto ResourceManager::load_texture(Resource const resource) -> gfx::Texture {
  if (!mResourceRegistry->has_resource(resource)) {
    mResourceRegistry->register_resource(resource);
  }

  if (is_loaded(resource)) {
    return mTextures[resource];
  }

  auto const& path{mResourceRegistry->get_path(resource)};

  return mTextures[resource] = mGfxResources->load_texture(path);
}

auto ResourceManager::load_x_model(Resource const resource)
  -> gfx::ext::XModel {
  if (!mResourceRegistry->has_resource(resource)) {
    mResourceRegistry->register_resource(resource);
  }

  if (is_loaded(resource)) {
    return mXModels[resource];
  }

  auto const& path{mResourceRegistry->get_path(resource)};

  return mXModels[resource] = mGfxResources->load_x_model(path);
}

auto ResourceManager::is_loaded(ResourceId const id) const -> bool {
  return mTextures.find(id) != mTextures.end() ||
         mXModels.find(id) != mXModels.end();
}

} // namespace basalt
