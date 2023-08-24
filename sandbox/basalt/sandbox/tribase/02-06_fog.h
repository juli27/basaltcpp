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
  explicit Fog(basalt::Engine&);

private:
  static constexpr basalt::u8 sNumTextures {6};

  basalt::gfx::ResourceCachePtr mGfxCache;
  basalt::gfx::Pipeline mPipeline;
  basalt::gfx::VertexBuffer mVertexBuffer;
  basalt::gfx::Sampler mSampler;
  std::array<basalt::gfx::Texture, sNumTextures> mTextures {};
  basalt::gfx::FogMode mFogMode {basalt::gfx::FogMode::Linear};
  bool mVertexFogRangeBased {true};
  bool mFragmentFog {false};
  basalt::Color mFogColor {basalt::Colors::BLACK};
  basalt::f32 mFogStart {25.0f};
  basalt::f32 mFogEnd {100.0f};
  basalt::f32 mFogDensity {0.01f};

  auto update_pipeline() -> void;
  auto render_ui() -> void;

  auto on_update(UpdateContext&) -> void override;
};

} // namespace tribase
