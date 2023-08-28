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

class BumpMapping final : public basalt::View {
public:
  explicit BumpMapping(const basalt::Engine&);

private:
  basalt::gfx::ResourceCachePtr mGfxCache;
  basalt::gfx::Pipeline mSkyBoxPipeline;
  basalt::gfx::Pipeline mCube1Pipeline;
  basalt::gfx::Pipeline mCube2Pipeline;
  basalt::gfx::Sampler mSampler;
  basalt::gfx::Texture mEnvTexture;
  basalt::gfx::Texture mBaseTexture;
  basalt::gfx::Texture mBumpMap;
  basalt::gfx::ext::XModel mCube;
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
