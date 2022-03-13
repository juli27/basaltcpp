#pragma once

#include <basalt/sandbox/test_case.h>

#include <basalt/api/gfx/camera.h>
#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>

namespace d3d9 {

struct Matrices final : TestCase {
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
  basalt::f32 mAngleYRad {};

  auto on_draw(basalt::gfx::ResourceCache&, basalt::Size2Du16 viewport,
               const basalt::RectangleU16& clip)
    -> std::tuple<basalt::gfx::CommandList, basalt::RectangleU16> override;

  void on_tick(basalt::Engine&) override;
};

} // namespace d3d9
