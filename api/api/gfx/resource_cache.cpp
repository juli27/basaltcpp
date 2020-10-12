#include "resource_cache.h"

#include "backend/device.h"
#include "backend/utils.h"
#include "backend/ext/x_model_support.h"

#include "api/resources/resource_registry.h"

#include "api/shared/asserts.h"
#include "api/shared/utils.h"

#include <utility>

using namespace std::literals;

namespace basalt::gfx {

ResourceCache::ResourceCache(ResourceRegistryPtr resourceRegistry,
                             DevicePtr device)
  : mResourceRegistry {std::move(resourceRegistry)}
  , mDevice {std::move(device)} {
}

void ResourceCache::load(const GfxModel model) const {
  auto& registry = mResourceRegistry->get<GfxModel>();

  const auto modelExt =
    *gfx::query_device_extension<ext::XModelSupport>(*mDevice);

  auto& location = registry.get<FileLocation>(model);
  registry.emplace<ext::ModelHandle>(model, modelExt->load(location.path));
}

void ResourceCache::load(const Texture texture) const {
  auto& registry = mResourceRegistry->get<Texture>();
  auto& location = registry.get<FileLocation>(texture);
  registry.emplace<TextureHandle>(texture, mDevice->add_texture(location.path));
}

void ResourceCache::load(const Material material) const {
  auto& registry = mResourceRegistry->get<Material>();

  const auto& descriptor = registry.get<MaterialDescriptor>(material);
  auto& data = registry.emplace<MaterialData>(material);
  data.textureStageStates[enum_cast(TextureStageState::CoordinateSource)] = 0;

  u32 value = 0;
  switch (descriptor.textureTransformMode) {
  case TextureTransformMode::Disabled:
    value = TtfDisabled;
    break;
  case TextureTransformMode::Count4:
    value = TtfCount4;
    break;
  }

  if (descriptor.textureTransformProjected) {
    value |= TtfProjected;
  }

  data.textureStageStates[enum_cast(TextureStageState::TextureTransformFlags)] =
    value;
}

auto ResourceCache::has(const Material material) const -> bool {
  auto& registry = mResourceRegistry->get<Material>();

  return registry.valid(material) && registry.has<MaterialData>(material);
}

auto ResourceCache::get(const GfxModel model) const -> ext::ModelHandle {
  auto& registry = mResourceRegistry->get<GfxModel>();

  return registry.get<ext::ModelHandle>(model);
}

auto ResourceCache::get(const Texture texture) const -> TextureHandle {
  auto& registry = mResourceRegistry->get<Texture>();

  if (!registry.valid(texture)) {
    return TextureHandle::null();
  }

  return registry.get<TextureHandle>(texture);
}

auto ResourceCache::get(const Material material) const -> const MaterialData& {
  auto& registry = mResourceRegistry->get<Material>();

  // TODO: return an empty default material
  BASALT_ASSERT(registry.valid(material));
  if (!registry.valid(material)) {
    static constexpr MaterialData DATA;

    return DATA;
  }

  return registry.get<MaterialData>(material);
}

} // namespace basalt::gfx
