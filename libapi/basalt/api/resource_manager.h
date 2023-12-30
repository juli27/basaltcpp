#pragma once

#include <basalt/api/types.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/gfx/backend/types.h>
#include <basalt/api/gfx/backend/ext/types.h>

#include <filesystem>
#include <unordered_map>

namespace basalt {

class ResourceManager final {
public:
  explicit ResourceManager(gfx::ResourceCachePtr);

  [[nodiscard]]
  auto load_texture(std::filesystem::path const&) -> gfx::Texture;

  [[nodiscard]]
  auto load_x_model(std::filesystem::path const&) -> gfx::ext::XModel;

private:
  gfx::ResourceCachePtr mGfxResources;
  std::unordered_map<std::filesystem::path, gfx::Texture> mTextures;
  std::unordered_map<std::filesystem::path, gfx::ext::XModel> mXModels;

  [[nodiscard]]
  auto is_loaded(std::filesystem::path const&) const -> bool;
};

} // namespace basalt
