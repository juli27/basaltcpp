#pragma once

#include <basalt/api/types.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>
#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/api/shared/types.h>

#include <array>

namespace tribase {

class MultiTexturing final : public basalt::View {
public:
  explicit MultiTexturing(basalt::Engine const&);

private:
  basalt::gfx::ResourceCachePtr mGfxCache;
  std::array<basalt::gfx::PipelineHandle, 12> mPipelines{};
  basalt::gfx::SamplerHandle mSampler;
  basalt::gfx::TextureHandle mTexture0;
  basalt::gfx::TextureHandle mTexture1;
  basalt::gfx::ext::XMeshHandle mCubeMesh;
  basalt::SecondsF32 mTime{};

  auto on_update(UpdateContext&) -> void override;
};

} // namespace tribase
