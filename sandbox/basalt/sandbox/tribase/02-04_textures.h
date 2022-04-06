#pragma once

#include <basalt/api/view.h>

#include <basalt/api/types.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/shared/types.h>

#include <basalt/api/math/vector3.h>

#include <basalt/api/base/types.h>

#include <vector>

namespace tribase {

class Textures final : public basalt::View {
public:
  explicit Textures(basalt::Engine&);

private:
  static constexpr auto sNumTriangles = basalt::u16{1024};

  struct TriangleData final {
    basalt::Vector3f32 position;
    basalt::Vector3f32 rotation;
    basalt::f32 scale{1.0f};
    basalt::Vector3f32 velocity;
    basalt::Vector3f32 rotationVelocity;
  };

  std::vector<TriangleData> mTriangles{sNumTriangles};
  basalt::gfx::ResourceCachePtr mGfxCache;
  basalt::gfx::SamplerHandle mSamplerPoint;
  basalt::gfx::SamplerHandle mSamplerLinearWithMip;
  basalt::gfx::SamplerHandle mSamplerAnisotropic;
  basalt::gfx::TextureHandle mTexture;
  basalt::gfx::VertexBufferHandle mVertexBuffer;
  basalt::gfx::PipelineHandle mPipeline;
  basalt::SecondsF32 mTime{};

  auto on_update(UpdateContext&) -> void override;

  auto on_input(basalt::InputEvent const&)
    -> basalt::InputEventHandled override;
};

} // namespace tribase
