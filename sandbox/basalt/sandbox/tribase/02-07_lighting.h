#pragma once

#include <basalt/api/types.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>
#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/api/shared/types.h>

namespace tribase {

class Lighting final : public basalt::View {
public:
  explicit Lighting(basalt::Engine const&);

private:
  basalt::gfx::ResourceCachePtr mGfxCache;
  basalt::gfx::ext::XMeshHandle mSphereMesh;
  basalt::gfx::ext::XMeshHandle mGroundMesh;
  basalt::gfx::TextureHandle mSphereTexture;
  basalt::gfx::TextureHandle mGroundTexture;
  basalt::gfx::PipelineHandle mPipeline;
  basalt::gfx::PipelineHandle mNoTexturePipeline;
  basalt::gfx::SamplerHandle mSampler;
  basalt::SecondsF32 mTime{};

  auto on_update(UpdateContext&) -> void override;
};

} // namespace tribase
