#pragma once

#include "api/base/types.h"

#include <memory>
#include <string>

namespace basalt {

enum class GfxModel : u32 {};
enum class Texture : u32 {};

struct ResourceRegistry;
using ResourceRegistryPtr = std::shared_ptr<ResourceRegistry>;

struct FileLocation final {
  std::string path;
};

// MATERIAL

enum class Material : u32 {};

enum class TextureTransformMode { Disabled, Count4 };

struct MaterialDescriptor final {
  TextureTransformMode textureTransformMode {TextureTransformMode::Disabled};
  bool textureTransformProjected {false};
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
