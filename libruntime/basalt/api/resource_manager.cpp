#include <basalt/api/resource_manager.h>

#include <basalt/api/gfx/resource_cache.h>

#include <filesystem>
#include <utility>

namespace basalt {

using std::filesystem::path;

ResourceManager::ResourceManager(gfx::ResourceCachePtr gfxResources)
  : mGfxResources{std::move(gfxResources)} {
}

auto ResourceManager::load_texture(path const& filePath) -> gfx::Texture {
  if (is_loaded(filePath)) {
    return mTextures[filePath];
  }

  return mTextures[filePath] = mGfxResources->load_texture(filePath);
}

auto ResourceManager::load_x_model(path const& filePath)
  -> gfx::ext::XModel {
  if (is_loaded(filePath)) {
    return mXModels[filePath];
  }

  return mXModels[filePath] = mGfxResources->load_x_model(filePath);
}

auto ResourceManager::is_loaded(path const& filePath) const -> bool {
  return mTextures.find(filePath) != mTextures.end() ||
         mXModels.find(filePath) != mXModels.end();
}

} // namespace basalt
