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
  std::array<basalt::gfx::Pipeline, 12> mPipelines{};
  basalt::gfx::Sampler mSampler;
  basalt::gfx::Texture mTexture0;
  basalt::gfx::Texture mTexture1;
  basalt::gfx::ext::XModel mCube;
  basalt::SecondsF32 mTime{};

  auto on_update(UpdateContext&) -> void override;
};

} // namespace tribase
