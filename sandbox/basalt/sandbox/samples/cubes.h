#pragma once

#include <basalt/api/types.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/scene/types.h>

namespace samples {

class Cubes final : public basalt::View {
public:
  explicit Cubes(basalt::Engine&);

  Cubes(const Cubes&) = delete;
  Cubes(Cubes&&) = delete;

  ~Cubes() noexcept override;

  auto operator=(const Cubes&) -> Cubes& = delete;
  auto operator=(Cubes&&) -> Cubes& = delete;

private:
  basalt::gfx::ResourceCache& mGfxCache;
  basalt::gfx::Mesh mMesh;
  basalt::gfx::Texture mTexture;
  basalt::gfx::Material mMaterial;
  basalt::ScenePtr mScene;
  basalt::EntityId mCameraId;
};

} // namespace samples