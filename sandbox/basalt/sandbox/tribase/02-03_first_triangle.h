#pragma once

#include <basalt/api/types.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/math/angle.h>

#include <basalt/api/base/types.h>

namespace tribase {

class FirstTriangle final : public basalt::View {
public:
  explicit FirstTriangle(basalt::Engine&);
  FirstTriangle(const FirstTriangle&) = delete;
  FirstTriangle(FirstTriangle&&) = delete;

  ~FirstTriangle() noexcept override;

  auto operator=(const FirstTriangle&) -> FirstTriangle& = delete;
  auto operator=(FirstTriangle&&) -> FirstTriangle& = delete;

private:
  basalt::f64 mTimeAccum {};
  basalt::gfx::ResourceCache& mGfxCache;
  basalt::gfx::Pipeline mPipeline;
  basalt::gfx::Pipeline mQuadPipeline;
  basalt::gfx::Pipeline mWireframePipeline;
  basalt::gfx::VertexBuffer mVertexBuffer;
  basalt::Angle mRotationY;
  basalt::f32 mScale {1.0f};
  basalt::i32 mCurrentExercise {};

  auto on_update(UpdateContext&) -> void override;
};

} // namespace tribase
