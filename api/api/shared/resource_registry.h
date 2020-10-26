#pragma once

#include "types.h"

#include <entt/core/hashed_string.hpp>

#include <filesystem>
#include <unordered_map>

namespace basalt {

struct ResourceRegistry final {
  void register_resource(entt::hashed_string file);

  [[nodiscard]] auto has_resource(ResourceId) const -> bool;
  [[nodiscard]] auto get_path(ResourceId) const -> const std::filesystem::path&;

private:
  std::unordered_map<ResourceId, std::filesystem::path> mResources;
};

} // namespace basalt
