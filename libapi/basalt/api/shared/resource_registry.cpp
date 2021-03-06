#include <basalt/api/shared/resource_registry.h>

#include <basalt/api/shared/asserts.h>

namespace basalt {

void ResourceRegistry::register_resource(const Resource resource) {
  mResources[resource] = resource.data();
}

auto ResourceRegistry::has_resource(const ResourceId id) const -> bool {
  return mResources.find(id) != mResources.end();
}

auto ResourceRegistry::get_path(const ResourceId id) const
  -> const std::filesystem::path& {
  BASALT_ASSERT(mResources.find(id) != mResources.end(), "invalid ResourceId");

  return mResources.at(id);
}

} // namespace basalt
