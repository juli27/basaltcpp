#pragma once

#include <basalt/api/view.h>

#include <basalt/api/types.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/base/types.h>

namespace tribase {

class TexturesExercises final : public basalt::View {
public:
  explicit TexturesExercises(basalt::Engine&);

  TexturesExercises(const TexturesExercises&) = delete;
  TexturesExercises(TexturesExercises&&) noexcept = default;

  ~TexturesExercises() noexcept override;

  auto operator=(const TexturesExercises&) -> TexturesExercises& = delete;
  auto operator=(TexturesExercises&&) noexcept -> TexturesExercises& = delete;

private:
  basalt::gfx::ResourceCache& mGfxCache;
  basalt::gfx::Texture mTexture;
  basalt::gfx::VertexBuffer mVertexBuffer;
  basalt::gfx::Pipeline mPipeline;
  basalt::gfx::Sampler mSampler;
  basalt::i32 mCurrentExercise {0};

  auto on_draw(const DrawContext&) -> void override;
  auto on_tick(basalt::Engine&) -> void override;
};

} // namespace tribase
