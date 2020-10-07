#pragma once

#include "api/shared/types.h"

#include <entt/entity/entity.hpp>

#include <memory>
#include <string>

namespace basalt {

ENTT_OPAQUE_TYPE(GfxModel, u32);
ENTT_OPAQUE_TYPE(Texture, u32);

struct ResourceRegistry;
using ResourceRegistryPtr = std::shared_ptr<ResourceRegistry>;

struct FileLocation final {
  std::string path;
};

// enum class Location {
//  GFX
//};

// using ResourceId = entt::hashed_string;

// void AddTexture(ResourceId id, std::string_view filePath);
// TextureHandle GetTextureHandle(Location location, ResourceId id);

// resources
// Model -> Mesh*, Material*, nodes
//       | DirectXModel
// Mesh -> vertices, indices, primitive type
// Material -> shader, effect file, variables, textures
// Texture -> pixel data, mip levels

// resource locations
//             | Texture | Material | Mesh | Model |
// Filesystem  |    x    |    x     |  x   |   x   |
// CPU         |    x    |    x     |  x   |   x   |
// GPU         |    x    |    x     |  x   |  (x)  |
// GPU Managed |    x    |    x     |  x   |  (x)  |

} // namespace basalt
