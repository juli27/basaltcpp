#pragma once

#include <basalt/api/view.h>

#include <basalt/api/types.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/gfx/backend/types.h>

namespace tribase {

class TexturesEx1 final : public basalt::View {
public:
  explicit TexturesEx1(basalt::Engine&);

  TexturesEx1(const TexturesEx1&) = delete;
  TexturesEx1(TexturesEx1&&) noexcept = default;

  ~TexturesEx1() noexcept override;

  auto operator=(const TexturesEx1&) -> TexturesEx1& = delete;
  auto operator=(TexturesEx1&&) noexcept -> TexturesEx1& = delete;

private:
  basalt::gfx::ResourceCache& mGfxCache;
  basalt::gfx::Texture mTexture;
  basalt::gfx::VertexBuffer mVertexBuffer;
  basalt::gfx::Pipeline mPipeline;
  basalt::gfx::Sampler mSampler;

  auto on_draw(const DrawContext&) -> void override;
};

} // namespace tribase
