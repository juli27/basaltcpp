#pragma once

#include <basalt/api/view.h>

#include <basalt/api/types.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/base/types.h>

namespace tribase {

class TexturesExercises final : public basalt::View {
public:
  explicit TexturesExercises(basalt::Engine const&);

private:
  basalt::gfx::ResourceCachePtr mGfxCache;
  basalt::gfx::SamplerHandle mSampler;
  basalt::gfx::TextureHandle mTexture;
  basalt::gfx::VertexBufferHandle mVertexBuffer;
  basalt::gfx::PipelineHandle mPipeline;
  basalt::i32 mCurrentExercise{0};

  auto on_update(UpdateContext&) -> void override;
};

} // namespace tribase
