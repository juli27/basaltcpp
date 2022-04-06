#pragma once

#include <basalt/api/types.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/shared/color.h>

#include <array>

namespace tribase {

class Fog final : public basalt::View {
public:
  explicit Fog(basalt::Engine const&);

private:
  static constexpr basalt::u8 sNumTextures{6};

  basalt::gfx::ResourceCachePtr mGfxCache;
  basalt::gfx::ResourceCachePtr mReloadableGfxResources;
  basalt::gfx::PipelineHandle mPipeline;
  basalt::gfx::VertexBufferHandle mVertexBuffer;
  basalt::gfx::SamplerHandle mSampler;
  std::array<basalt::gfx::TextureHandle, sNumTextures> mTextures{};
  basalt::gfx::FogMode mFogMode{basalt::gfx::FogMode::Linear};
  bool mVertexFogRangeBased{true};
  bool mFragmentFog{false};
  basalt::Color mFogColor{basalt::Colors::BLACK};
  basalt::f32 mFogStart{25.0f};
  basalt::f32 mFogEnd{100.0f};
  basalt::f32 mFogDensity{0.01f};

  auto update_pipeline(basalt::gfx::Context&) -> void;
  auto render_ui(basalt::gfx::Context&) -> void;

  auto on_update(UpdateContext&) -> void override;
};

} // namespace tribase
