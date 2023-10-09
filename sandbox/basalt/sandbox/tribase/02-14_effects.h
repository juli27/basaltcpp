#pragma once

#include <basalt/api/types.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>
#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/api/shared/types.h>

#include <gsl/zstring>

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace tribase {

class Effects final : public basalt::View {
public:
  explicit Effects(basalt::Engine const&);

private:
  struct LoadedEffect {
    static auto compile(std::filesystem::path const&, basalt::Engine const&)
      -> std::optional<LoadedEffect>;

    basalt::gfx::ResourceCachePtr gfxCache;
    basalt::gfx::ext::EffectId id;
    std::string description;
    basalt::gfx::Texture backgroundTexture;
    basalt::gfx::ext::XModel model;
    gsl::czstring activeTechniqueName;
    basalt::u32 activeTechniqueNumPasses;
  };

  basalt::gfx::ResourceCachePtr mGfxCache;
  std::vector<std::filesystem::path> mEffectFilePaths;
  std::filesystem::path mEffectFilePath;
  LoadedEffect mLoadedEffect;
  basalt::gfx::VertexBuffer mRectVb;
  basalt::gfx::Sampler mLinearSampler;
  basalt::gfx::Texture mBackgroundTex;
  basalt::gfx::Pipeline mBackgroundPipeline;
  basalt::gfx::Pipeline mDefaultPipeline;
  basalt::SecondsF32 mTime{0};

  auto list_effect_files() -> void;

  auto on_update(UpdateContext&) -> void override;
};

} // namespace tribase
