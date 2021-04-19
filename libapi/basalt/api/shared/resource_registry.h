#pragma once

#include <basalt/api/shared/types.h>

#include <filesystem>
#include <unordered_map>

namespace basalt {

struct ResourceRegistry final {
  void register_resource(Resource);

  [[nodiscard]] auto has_resource(ResourceId) const -> bool;
  [[nodiscard]] auto get_path(ResourceId) const -> const std::filesystem::path&;

private:
  std::unordered_map<ResourceId, std::filesystem::path> mResources;
};

} // namespace basalt
