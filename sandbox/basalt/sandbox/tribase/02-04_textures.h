#pragma once

#include <basalt/api/view.h>

#include <basalt/api/types.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/math/vector3.h>

#include <basalt/api/base/types.h>

#include <vector>

namespace tribase {

class Textures final : public basalt::View {
public:
  explicit Textures(basalt::Engine&);

private:
  struct TriangleData final {
    basalt::Vector3f32 position;
    basalt::Vector3f32 rotation;
    basalt::f32 scale {1.0f};
    basalt::Vector3f32 velocity;
    basalt::Vector3f32 rotationVelocity;
  };

  std::vector<TriangleData> mTriangles;
  basalt::gfx::ResourceCachePtr mGfxCache;
  basalt::gfx::Pipeline mPipeline;
  basalt::gfx::VertexBuffer mVertexBuffer;
  basalt::gfx::Texture mTexture;
  basalt::gfx::Sampler mSamplerPoint;
  basalt::gfx::Sampler mSamplerLinearWithMip;
  basalt::gfx::Sampler mSamplerAnisotropic;
  basalt::f64 mTimeAccum {};

  auto on_update(UpdateContext&) -> void override;

  auto on_input(const basalt::InputEvent&)
    -> basalt::InputEventHandled override;
};

} // namespace tribase
