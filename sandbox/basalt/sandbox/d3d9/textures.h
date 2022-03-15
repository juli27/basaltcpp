#pragma once

#include <basalt/api/view.h>

#include <basalt/api/gfx/camera.h>
#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/base/types.h>

namespace d3d9 {

struct Textures final : basalt::View {
  explicit Textures(basalt::Engine&);
  Textures(const Textures&) = delete;
  Textures(Textures&&) = delete;

  ~Textures() noexcept override;

  auto operator=(const Textures&) -> Textures& = delete;
  auto operator=(Textures&&) -> Textures& = delete;

private:
  basalt::gfx::ResourceCache& mResourceCache;
  basalt::gfx::Pipeline mPipeline {basalt::gfx::Pipeline::null()};
  basalt::gfx::VertexBuffer mVertexBuffer {basalt::gfx::VertexBuffer::null()};
  basalt::gfx::Sampler mSampler {basalt::gfx::Sampler::null()};
  basalt::gfx::Texture mTexture {basalt::gfx::Texture::null()};
  basalt::gfx::Camera mCamera;
  basalt::f32 mAngleXRad {};

  auto on_draw(const DrawContext&) -> void override;

  void on_tick(basalt::Engine&) override;
};

} // namespace d3d9
