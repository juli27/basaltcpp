#pragma once

#include <basalt/api/types.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>
#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/api/shared/types.h>

#include <array>

namespace tribase {

class Blending final : public basalt::View {
public:
  explicit Blending(basalt::Engine const&);

private:
  static constexpr auto sNumPlanetTextures = basalt::u8{6};

  basalt::gfx::ResourceCachePtr mGfxCache;
  basalt::gfx::PipelineHandle mStarPipeline;
  basalt::gfx::PipelineHandle mPlanetPipelineCw;
  basalt::gfx::PipelineHandle mPlanetPipelineCcw;
  basalt::gfx::SamplerHandle mSampler;
  std::array<basalt::gfx::TextureHandle, sNumPlanetTextures> mPlanetTextures{};
  basalt::gfx::ext::XMeshHandle mPlanetMesh;
  basalt::gfx::ext::XMeshHandle mSunMesh;
  basalt::gfx::VertexBufferHandle mStarsVb;
  basalt::SecondsF32 mTime{};

  auto on_update(UpdateContext&) -> void override;
};

} // namespace tribase
