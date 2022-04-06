#pragma once

#include <basalt/api/types.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/shared/types.h>

#include <basalt/api/math/angle.h>

#include <basalt/api/base/types.h>

namespace tribase {

class FirstTriangle final : public basalt::View {
public:
  explicit FirstTriangle(basalt::Engine const&);

private:
  basalt::gfx::ResourceCachePtr mGfxCache;
  basalt::gfx::VertexBufferHandle mVertexBuffer;
  basalt::gfx::PipelineHandle mPipeline;
  basalt::gfx::PipelineHandle mQuadPipeline;
  basalt::gfx::PipelineHandle mWireframePipeline;
  basalt::SecondsF32 mTime{};
  basalt::Angle mRotationY;
  basalt::f32 mScale{1.0f};
  basalt::i32 mCurrentExercise{};

  auto on_update(UpdateContext&) -> void override;
};

} // namespace tribase
