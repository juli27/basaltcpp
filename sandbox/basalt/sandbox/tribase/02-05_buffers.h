#pragma once

#include <basalt/api/types.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/vector3.h>

namespace tribase {

class Buffers final : public basalt::View {
public:
  explicit Buffers(basalt::Engine const&);

private:
  basalt::gfx::ResourceCachePtr mGfxCache;
  basalt::gfx::Sampler mSampler;
  basalt::gfx::Texture mTexture;
  basalt::gfx::VertexBuffer mVertexBuffer;
  basalt::gfx::IndexBuffer mIndexBuffer;
  basalt::gfx::Pipeline mPipeline;
  basalt::Vector3f32 mCameraPos{0.0f};
  basalt::Angle mCameraAngleY{};
  basalt::Angle mFov;

  auto on_update(UpdateContext&) -> void override;
  auto on_input(basalt::InputEvent const&)
    -> basalt::InputEventHandled override;
};

} // namespace tribase
