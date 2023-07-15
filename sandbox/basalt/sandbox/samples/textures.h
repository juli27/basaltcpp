#pragma once

#include <basalt/api/view.h>

#include <basalt/api/types.h>

#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>

#include <array>

namespace samples {

// TODO: when supporting dynamic buffers: texture coordinate modification and
// address mode demo
class Textures final : public basalt::View {
public:
  explicit Textures(basalt::Engine&);

private:
  basalt::gfx::ResourceCachePtr mGfxCache;
  basalt::gfx::Mesh mMesh;
  basalt::gfx::Texture mTexture;
  std::array<basalt::gfx::Material, 9> mMaterials {};
};

} // namespace samples
