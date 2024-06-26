#pragma once

#include <basalt/api/types.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/shared/types.h>

namespace tribase {

class VolumeTextures final : public basalt::View {
public:
  explicit VolumeTextures(basalt::Engine const&);

private:
  basalt::gfx::ResourceCachePtr mGfxCache;
  basalt::gfx::PipelineHandle mPipeline;
  basalt::gfx::SamplerHandle mSampler;
  basalt::gfx::TextureHandle mExplosionTexture;
  basalt::gfx::VertexBufferHandle mVertexBuffer;
  basalt::SecondsF32 mTime{};

  auto on_update(UpdateContext&) -> void override;
};

} // namespace tribase
