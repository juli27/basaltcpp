#pragma once

#include <basalt/api/types.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>
#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/vector3.h>

#include <basalt/api/base/types.h>

namespace tribase {

class EnvMapping final : public basalt::View {
public:
  explicit EnvMapping(basalt::Engine const&);

private:
  basalt::gfx::ResourceCachePtr mGfxCache;
  basalt::gfx::PipelineHandle mSkyBoxPipeline;
  basalt::gfx::PipelineHandle mSphere1Pipeline;
  basalt::gfx::PipelineHandle mSphere2Pipeline;
  basalt::gfx::PipelineHandle mSphere3Pipeline;
  basalt::gfx::SamplerHandle mSampler;
  basalt::gfx::TextureHandle mEnvTexture;
  basalt::gfx::TextureHandle mTexture;
  basalt::gfx::ext::XMeshHandle mSphereMesh;
  basalt::gfx::VertexBufferHandle mSkyBoxVb;
  basalt::gfx::IndexBufferHandle mSkyBoxIb;
  basalt::Vector3f32 mCameraPos{};
  basalt::Angle mCameraAngleY{};
  basalt::f32 mCameraUpDown{};

  auto on_update(UpdateContext&) -> void override;
  auto on_input(basalt::InputEvent const&)
    -> basalt::InputEventHandled override;
};

} // namespace tribase
