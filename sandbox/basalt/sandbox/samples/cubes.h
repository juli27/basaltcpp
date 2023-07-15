#pragma once

#include <basalt/api/types.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/scene/types.h>

namespace samples {

class Cubes final : public basalt::View {
public:
  explicit Cubes(basalt::Engine&);

private:
  basalt::gfx::ResourceCachePtr mGfxCache;
  basalt::gfx::Mesh mMesh;
  basalt::gfx::Texture mTexture;
  basalt::gfx::Material mMaterial;
  basalt::ScenePtr mScene;
  basalt::EntityId mCameraId;
};

} // namespace samples