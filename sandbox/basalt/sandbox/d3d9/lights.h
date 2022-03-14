#pragma once

#include <basalt/api/view.h>

#include <basalt/api/gfx/camera.h>
#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>

namespace d3d9 {

struct Lights final : basalt::View {
  explicit Lights(basalt::Engine&);
  Lights(const Lights&) = delete;
  Lights(Lights&&) = delete;

  ~Lights() noexcept override;

  auto operator=(const Lights&) -> Lights& = delete;
  auto operator=(Lights&&) -> Lights& = delete;

private:
  basalt::gfx::ResourceCache& mResourceCache;
  basalt::gfx::Pipeline mPipeline {basalt::gfx::Pipeline::null()};
  basalt::gfx::VertexBuffer mVertexBuffer {basalt::gfx::VertexBuffer::null()};
  basalt::gfx::Camera mCamera;
  basalt::f32 mAngleXRad {};
  basalt::f32 mLightAngle {};

  auto on_draw(basalt::gfx::ResourceCache&, basalt::Size2Du16 viewport)
    -> basalt::gfx::CommandList override;

  void on_tick(basalt::Engine&) override;
};

} // namespace d3d9
