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
  basalt::gfx::SamplerHandle mSampler;
  basalt::gfx::TextureHandle mTexture;
  basalt::gfx::VertexBufferHandle mVertexBuffer;
  basalt::gfx::IndexBufferHandle mIndexBuffer;
  basalt::gfx::PipelineHandle mPipeline;
  basalt::Vector3f32 mCameraPos{0.0f};
  basalt::Angle mCameraAngleY{};
  basalt::Angle mFov;

  auto on_update(UpdateContext&) -> void override;
  auto on_input(basalt::InputEvent const&)
    -> basalt::InputEventHandled override;
};

} // namespace tribase
