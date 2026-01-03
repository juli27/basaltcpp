#pragma once

#include "backend/pipeline.h" // IWYU pragma: export

#include "context.h"
#include "types.h"
#include "backend/types.h"

#include <basalt/api/base/enum_set.h>
#include <basalt/api/base/types.h>

#include <gsl/span>

#include <vector>

namespace basalt::gfx {

struct MaterialClassCreateInfo {
  PipelineCreateInfo pipelineInfo;
};

enum class MaterialFeature : u8 {
  DepthBuffer,
  Lighting,
};
using MaterialFeatures = EnumSet<MaterialFeature, MaterialFeature::Lighting>;

[[nodiscard]]
auto collect_material_features(MaterialClassCreateInfo const&)
  -> MaterialFeatures;

enum class MaterialPropertyType : u8 {
  UniformColors,
  FogParameters,
  SampledTexture,
  Matrix4x4F32,
};
enum class MaterialPropertyId : u8 {
  UniformColors,
  FogParameters,
  SampledTexture,
  TexTransform,
};

struct MaterialPropertyInfo {
  MaterialPropertyId id;
  MaterialPropertyType type;
};

[[nodiscard]]
auto collect_material_properties(MaterialClassCreateInfo const&)
  -> std::vector<MaterialPropertyInfo>;

class MaterialClass {
public:
  MaterialClass(Pipeline, MaterialFeatures, std::vector<MaterialPropertyInfo>);

  [[nodiscard]]
  auto features() const noexcept -> MaterialFeatures;
  [[nodiscard]]
  auto properties() const noexcept -> gsl::span<MaterialPropertyInfo const>;
  [[nodiscard]]
  auto pipeline() const noexcept -> PipelineHandle;

private:
  Pipeline mPipeline;
  // TODO: Input and output requirements
  MaterialFeatures mFeatures;
  std::vector<MaterialPropertyInfo> mProperties;
};

} // namespace basalt::gfx
