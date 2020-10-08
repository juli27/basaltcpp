#pragma once

#include "backend/types.h"

#include "backend/ext/x_model_support.h"

#include "api/resources/types.h"

namespace basalt::gfx {

struct ResourceCache {
  explicit ResourceCache(ResourceRegistryPtr, DevicePtr);

  void load(GfxModel) const;
  void load(Texture) const;

  [[nodiscard]] auto get(GfxModel) const -> ext::ModelHandle;
  [[nodiscard]] auto get(Texture) const -> TextureHandle;

private:
  ResourceRegistryPtr mResourceRegistry;
  DevicePtr mDevice;
};

} // namespace basalt::gfx
