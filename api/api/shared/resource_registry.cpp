#include "resource_registry.h"

#include "api/shared/asserts.h"

namespace basalt {

void ResourceRegistry::register_resource(const Resource resource) {
  mResources[resource] = resource.data();
}

auto ResourceRegistry::has_resource(const ResourceId id) const -> bool {
  return mResources.find(id) != mResources.end();
}

auto ResourceRegistry::get_path(const ResourceId id) const
  -> const std::filesystem::path& {
  BASALT_ASSERT_MSG(mResources.find(id) != mResources.end(),
                    "invalid ResourceId");

  return mResources.at(id);
}

} // namespace basalt
