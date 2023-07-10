#pragma once

#include <basalt/api/view.h>

#include <basalt/api/types.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/base/types.h>

#include <vector>

namespace tribase {

class Textures final : public basalt::View {
public:
  explicit Textures(basalt::Engine&);

  Textures(const Textures&) = delete;
  Textures(Textures&&) = delete;

  ~Textures() noexcept override;

  auto operator=(const Textures&) -> Textures& = delete;
  auto operator=(Textures&&) -> Textures& = delete;

private:
  struct TriangleData;

  std::vector<TriangleData> mTriangles;
  basalt::gfx::ResourceCache& mGfxCache;
  basalt::gfx::Pipeline mPipeline;
  basalt::gfx::VertexBuffer mVertexBuffer;
  basalt::gfx::Texture mTexture;
  basalt::gfx::Sampler mSamplerPoint;
  basalt::gfx::Sampler mSamplerLinearWithMip;
  basalt::gfx::Sampler mSamplerAnisotropic;
  basalt::f64 mTimeAccum {};

  auto on_update(UpdateContext&) -> void override;

  auto on_input(const basalt::InputEvent&)
    -> basalt::InputEventHandled override;
};

} // namespace tribase
