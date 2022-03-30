#pragma once

#include <basalt/api/view.h>

#include <basalt/api/gfx/camera.h>
#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/math/angle.h>

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
  basalt::Angle mRotationX {};
  basalt::Angle mLightRotation {};

  auto on_draw(const DrawContext&) -> void override;

  void on_tick(basalt::Engine&) override;
};

} // namespace d3d9
