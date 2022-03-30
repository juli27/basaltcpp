#pragma once

#include <basalt/api/view.h>

#include <basalt/api/gfx/camera.h>
#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/math/angle.h>

namespace d3d9 {

struct Matrices final : basalt::View {
  explicit Matrices(basalt::Engine&);

  Matrices(const Matrices&) = delete;
  Matrices(Matrices&&) = default;

  ~Matrices() noexcept override;

  auto operator=(const Matrices&) -> Matrices& = delete;
  auto operator=(Matrices&&) -> Matrices& = delete;

private:
  basalt::gfx::ResourceCache& mResourceCache;
  basalt::gfx::Pipeline mPipeline {basalt::gfx::Pipeline::null()};
  basalt::gfx::VertexBuffer mVertexBuffer {basalt::gfx::VertexBuffer::null()};
  basalt::gfx::Camera mCamera;
  basalt::Angle mRotationY {};

  auto on_draw(const DrawContext&) -> void override;

  void on_tick(basalt::Engine&) override;
};

} // namespace d3d9
