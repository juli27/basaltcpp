#pragma once

#include <basalt/api/types.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/math/angle.h>

#include <basalt/api/base/types.h>

namespace tribase {

struct Dreieck final : basalt::View {
  explicit Dreieck(basalt::Engine&);
  Dreieck(const Dreieck&) = delete;
  Dreieck(Dreieck&&) = delete;

  ~Dreieck() noexcept override;

  auto operator=(const Dreieck&) -> Dreieck& = delete;
  auto operator=(Dreieck&&) -> Dreieck& = delete;

private:
  basalt::f64 mTimeAccum {};
  basalt::gfx::ResourceCache& mResourceCache;
  basalt::gfx::Pipeline mPipeline;
  basalt::gfx::Pipeline mQuadPipeline;
  basalt::gfx::Pipeline mWireframePipeline;
  basalt::gfx::VertexBuffer mVertexBuffer;
  basalt::Angle mRotationY;
  basalt::f32 mScale {1.0f};
  basalt::i32 mCurrentExercise {};

  auto on_draw(const DrawContext&) -> void override;

  auto on_tick(basalt::Engine&) -> void override;
};

} // namespace tribase
