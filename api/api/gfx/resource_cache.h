#pragma once

#include "types.h"
#include "backend/types.h"
#include "backend/ext/types.h"

#include "api/shared/handle_pool.h"
#include "api/shared/types.h"

#include <unordered_map>

namespace basalt::gfx {

struct ResourceCache {
  explicit ResourceCache(ResourceRegistryPtr, DevicePtr);

  template <typename T>
  auto load(ResourceId) -> T;

  [[nodiscard]] auto is_loaded(ResourceId) const -> bool;

  template <typename T>
  auto get(ResourceId) -> T;

  auto create_material(const MaterialDescriptor&) -> Material;

  [[nodiscard]] auto get(Material) const -> const MaterialData&;

private:
  ResourceRegistryPtr mResourceRegistry;
  DevicePtr mDevice;
  std::unordered_map<ResourceId, ext::XModel> mModels;
  std::unordered_map<ResourceId, Texture> mTextures;
  HandlePool<MaterialData, Material> mMaterials;
};

template <>
[[nodiscard]] auto ResourceCache::load(ResourceId) -> ext::XModel;
template <>
[[nodiscard]] auto ResourceCache::load(ResourceId) -> Texture;

template <>
[[nodiscard]] auto ResourceCache::get(ResourceId) -> ext::XModel;
template <>
[[nodiscard]] auto ResourceCache::get(ResourceId) -> Texture;

} // namespace basalt::gfx
