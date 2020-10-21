#pragma once

#include "types.h"
#include "backend/types.h"
#include "backend/ext/types.h"

#include "api/resources/types.h"

namespace basalt::gfx {

struct ResourceCache {
  explicit ResourceCache(ResourceRegistryPtr, DevicePtr);

  void load(ext::XModel) const;
  void load(Texture) const;
  void load(Material) const;

  [[nodiscard]] auto has(Texture) const -> bool;
  [[nodiscard]] auto has(Material) const -> bool;

  [[nodiscard]] auto get(ext::XModel) const -> ext::XModelHandle;
  [[nodiscard]] auto get(Texture) const -> TextureHandle;
  [[nodiscard]] auto get(Material) const -> const MaterialData&;

private:
  ResourceRegistryPtr mResourceRegistry;
  DevicePtr mDevice;
};

} // namespace basalt::gfx
