#include <basalt/api/gfx/material_class.h>

#include <algorithm>
#include <utility>

auto basalt::gfx::collect_material_features(MaterialClassCreateInfo const& info)
  -> MaterialFeatures {
  auto const& pipelineInfo = info.pipelineInfo;
  auto features = MaterialFeatures{};

  if (pipelineInfo.depthTest != TestPassCond::Always ||
      pipelineInfo.depthWriteEnable) {
    features.set(MaterialFeature::DepthBuffer);
  }

  if (pipelineInfo.vertexShader) {
    auto const& vertexShader = *pipelineInfo.vertexShader;

    if (vertexShader.lightingEnabled) {
      features.set(MaterialFeature::Lighting);
    }
  }

  return features;
}

auto basalt::gfx::collect_material_properties(
  MaterialClassCreateInfo const& info) -> std::vector<MaterialPropertyInfo> {
  auto properties = std::vector<MaterialPropertyInfo>{};

  auto const& pipelineInfo = info.pipelineInfo;
  auto const* vs = pipelineInfo.vertexShader;
  auto needsFogParams = false;
  if (vs) {
    if (vs->lightingEnabled &&
        (vs->diffuseSource == MaterialColorSource::Material ||
         vs->ambientSource == MaterialColorSource::Material ||
         vs->emissiveSource == MaterialColorSource::Material ||
         vs->specularEnabled)) {
      properties.push_back(
        MaterialPropertyInfo{MaterialPropertyId::UniformColors,
                             MaterialPropertyType::UniformColors});
    }

    if (vs->fog != FogMode::None) {
      needsFogParams = true;
    }

    auto const needsTexTransform = [&] {
      auto const& texCoordSets = vs->textureCoordinateSets;

      return std::any_of(texCoordSets.begin(), texCoordSets.end(),
                         [](TextureCoordinateSet const& set) {
                           return set.transformMode !=
                                  TextureCoordinateTransformMode::Disabled;
                         });
    }();
    if (needsTexTransform) {
      properties.push_back(MaterialPropertyInfo{
        MaterialPropertyId::TexTransform, MaterialPropertyType::Matrix4x4F32});
    }
  }

  auto const* fs = pipelineInfo.fragmentShader;
  if (fs) {
    auto const stages = fs->textureStages;
    auto const needsTexture =
      std::any_of(stages.begin(), stages.end(), [](TextureStage const& stage) {
        return stage.colorArg1.src == TextureStageSrc::SampledTexture ||
               stage.colorArg2.src == TextureStageSrc::SampledTexture ||
               stage.colorArg3.src == TextureStageSrc::SampledTexture ||
               stage.alphaArg1.src == TextureStageSrc::SampledTexture ||
               stage.alphaArg2.src == TextureStageSrc::SampledTexture ||
               stage.alphaArg3.src == TextureStageSrc::SampledTexture;
      });
    if (needsTexture) {
      properties.push_back(
        MaterialPropertyInfo{MaterialPropertyId::SampledTexture,
                             MaterialPropertyType::SampledTexture});
    }

    if (fs->fog != FogMode::None) {
      needsFogParams = true;
    }
  }

  if (needsFogParams) {
    properties.push_back(MaterialPropertyInfo{
      MaterialPropertyId::FogParameters, MaterialPropertyType::FogParameters});
  }

  return properties;
}

namespace basalt::gfx {

MaterialClass::MaterialClass(Pipeline pipeline, MaterialFeatures const features,
                             std::vector<MaterialPropertyInfo> properties)
  : mPipeline{std::move(pipeline)}
  , mFeatures{features}
  , mProperties{std::move(properties)} {
}

auto MaterialClass::features() const noexcept -> MaterialFeatures {
  return mFeatures;
}

auto MaterialClass::properties() const noexcept
  -> gsl::span<MaterialPropertyInfo const> {
  return mProperties;
}

auto MaterialClass::pipeline() const noexcept -> PipelineHandle {
  return mPipeline;
}

} // namespace basalt::gfx
