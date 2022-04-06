#pragma once

#include <basalt/api/types.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/gfx/backend/types.h>
#include <basalt/api/gfx/backend/ext/types.h>

#include <unordered_map>

namespace basalt {

class ResourceManager final {
public:
  ResourceManager(ResourceRegistryPtr, gfx::ResourceCachePtr);

  [[nodiscard]] auto load_texture(Resource) -> gfx::Texture;

  [[nodiscard]] auto load_x_model(Resource) -> gfx::ext::XModel;

private:
  ResourceRegistryPtr mResourceRegistry;
  gfx::ResourceCachePtr mGfxResources;
  std::unordered_map<ResourceId, gfx::Texture> mTextures;
  std::unordered_map<ResourceId, gfx::ext::XModel> mXModels;

  [[nodiscard]] auto is_loaded(ResourceId) const -> bool;
};

} // namespace basalt
