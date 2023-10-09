#include <basalt/api/shared/resource_registry.h>

#include <basalt/api/shared/asserts.h>

namespace basalt {

auto ResourceRegistry::register_resource(Resource const resource) -> void {
  mResources[resource] = resource.data();
}

auto ResourceRegistry::has_resource(ResourceId const id) const -> bool {
  return mResources.find(id) != mResources.end();
}

auto ResourceRegistry::get_path(ResourceId const id) const
  -> std::filesystem::path const& {
  BASALT_ASSERT(mResources.find(id) != mResources.end(), "invalid ResourceId");

  return mResources.at(id);
}

} // namespace basalt
