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
  explicit Buffers(basalt::Engine&);
  Buffers(const Buffers&) = delete;
  Buffers(Buffers&&) = delete;

  ~Buffers() noexcept override;

  auto operator=(const Buffers&) -> Buffers& = delete;
  auto operator=(Buffers&&) -> Buffers& = delete;

private:
  basalt::gfx::ResourceCache& mResourceCache;
  basalt::gfx::Pipeline mPipeline;
  basalt::gfx::VertexBuffer mVertexBuffer;
  basalt::gfx::IndexBuffer mIndexBuffer;
  basalt::gfx::Sampler mSampler;
  basalt::gfx::Texture mTexture;
  basalt::Vector3f32 mCameraPos {0.0f};
  basalt::Angle mCameraAngleY {};
  basalt::Angle mFov;

  auto on_update(UpdateContext&) -> void override;
  auto on_input(const basalt::InputEvent&)
    -> basalt::InputEventHandled override;
};

} // namespace tribase
