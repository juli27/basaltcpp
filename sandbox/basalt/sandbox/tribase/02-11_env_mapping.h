#pragma once

#include <basalt/api/types.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>
#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/api/shared/types.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/vector3.h>

#include <basalt/api/base/types.h>

namespace tribase {

class EnvMapping final : public basalt::View {
public:
  explicit EnvMapping(const basalt::Engine&);

private:
  basalt::gfx::ResourceCachePtr mGfxCache;
  basalt::gfx::Pipeline mSkyBoxPipeline;
  basalt::gfx::Pipeline mSphere1Pipeline;
  basalt::gfx::Pipeline mSphere2Pipeline;
  basalt::gfx::Pipeline mSphere3Pipeline;
  basalt::gfx::Sampler mSampler;
  basalt::gfx::Texture mEnvTexture;
  basalt::gfx::Texture mTexture;
  basalt::gfx::ext::XModel mSphere;
  basalt::gfx::VertexBuffer mSkyBoxVb;
  basalt::gfx::IndexBuffer mSkyBoxIb;
  basalt::SecondsF32 mTime {};
  basalt::Vector3f32 mCameraPos {};
  basalt::Angle mCameraAngleY {};
  basalt::f32 mCameraUpDown {};

  auto on_update(UpdateContext&) -> void override;
  auto on_input(const basalt::InputEvent&) -> basalt::InputEventHandled override;
};

} // namespace tribase
