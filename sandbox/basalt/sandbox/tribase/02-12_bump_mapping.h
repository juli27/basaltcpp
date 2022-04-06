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
  explicit BumpMapping(basalt::Engine const&);

private:
  basalt::gfx::ResourceCachePtr mGfxCache;
  basalt::gfx::PipelineHandle mSkyBoxPipeline;
  basalt::gfx::PipelineHandle mCube1Pipeline;
  basalt::gfx::PipelineHandle mCube2Pipeline;
  basalt::gfx::SamplerHandle mSampler;
  basalt::gfx::TextureHandle mEnvTexture;
  basalt::gfx::TextureHandle mBaseTexture;
  basalt::gfx::TextureHandle mBumpMap;
  basalt::gfx::ext::XMeshHandle mCubeMesh;
  basalt::gfx::VertexBufferHandle mSkyBoxVb;
  basalt::gfx::IndexBufferHandle mSkyBoxIb;
  basalt::SecondsF32 mTime{};
  basalt::Vector3f32 mCameraPos{};
  basalt::Angle mCameraAngleY{};
  basalt::f32 mCameraUpDown{};

  auto on_update(UpdateContext&) -> void override;
  auto on_input(basalt::InputEvent const&)
    -> basalt::InputEventHandled override;
};

} // namespace tribase
