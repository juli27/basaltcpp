#pragma once

#include <basalt/api/shared/types.h>

#include <filesystem>
#include <unordered_map>

namespace basalt {

class ResourceRegistry final {
public:
  auto register_resource(Resource) -> void;

  [[nodiscard]] auto has_resource(ResourceId) const -> bool;
  [[nodiscard]] auto get_path(ResourceId) const -> std::filesystem::path const&;

private:
  std::unordered_map<ResourceId, std::filesystem::path> mResources{};
};

} // namespace basalt
