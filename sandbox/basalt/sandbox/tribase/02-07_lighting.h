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
  explicit Lighting(basalt::Engine&);

  Lighting(const Lighting&) = delete;
  Lighting(Lighting&&) = delete;

  ~Lighting() noexcept override;

  auto operator=(const Lighting&) -> Lighting& = delete;
  auto operator=(Lighting&&) -> Lighting& = delete;

private:
  basalt::gfx::ResourceCachePtr mGfxCache;
  basalt::gfx::Texture mSphereTexture;
  basalt::gfx::Texture mGroundTexture;
  basalt::gfx::ext::XModel mSphereModel;
  basalt::gfx::ext::XModel mGroundModel;
  basalt::gfx::Pipeline mPipeline;
  basalt::gfx::Pipeline mNoTexturePipeline;
  basalt::gfx::Sampler mSampler;
  basalt::SecondsF32 mTime {};

  auto on_update(UpdateContext&) -> void override;
};

} // namespace tribase
