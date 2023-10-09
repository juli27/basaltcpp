#pragma once

#include <basalt/api/types.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/vector3.h>

#include <basalt/api/base/types.h>

#include <vector>

namespace tribase {

class BuffersExercises final : public basalt::View {
public:
  explicit BuffersExercises(basalt::Engine const&);

private:
  struct CubeData final {
    basalt::Vector3f32 position;
    basalt::Vector3f32 velocity;
  };

  std::vector<CubeData> mCubes;
  basalt::gfx::ResourceCachePtr mGfxCache;
  basalt::gfx::Pipeline mPipeline;
  basalt::gfx::VertexBuffer mVertexBuffer;
  basalt::gfx::IndexBuffer mIndexBuffer;
  basalt::gfx::Sampler mSampler;
  basalt::gfx::Texture mTexture;
  basalt::Vector3f32 mCameraPos{0.0f};
  basalt::Angle mCameraAngleY{};
  basalt::Angle mFov;
  basalt::i32 mCurrentExercise{0};

  auto regenerate_velocities() -> void;

  auto on_update(UpdateContext&) -> void override;
  auto on_input(basalt::InputEvent const&)
    -> basalt::InputEventHandled override;
};

} // namespace tribase
