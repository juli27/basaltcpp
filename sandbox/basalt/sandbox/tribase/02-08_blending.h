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
  basalt::gfx::Pipeline mStarPipeline;
  basalt::gfx::Pipeline mPlanetPipelineCw;
  basalt::gfx::Pipeline mPlanetPipelineCcw;
  basalt::gfx::Sampler mSampler;
  std::array<basalt::gfx::Texture, sNumPlanetTextures> mPlanetTextures{};
  basalt::gfx::ext::XModel mPlanetModel;
  basalt::gfx::ext::XModel mSunModel;
  basalt::gfx::VertexBuffer mStarsVb;
  basalt::SecondsF32 mTime{};

  auto on_update(UpdateContext&) -> void override;
};

} // namespace tribase
