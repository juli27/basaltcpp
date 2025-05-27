#pragma once

#include "resources.h"
#include "types.h"
#include "backend/types.h"

#include <basalt/api/base/enum_set.h>
#include <basalt/api/base/types.h>

namespace basalt::gfx {

struct MaterialClassCreateInfo {
  PipelineCreateInfo pipelineInfo;
};

enum class MaterialFeature : u8 {
  TexCoordTransform,
  Lighting,
  UniformColors,
  Fog,
  Texturing,
  DepthBuffer,
};
using MaterialFeatures = EnumSet<MaterialFeature, MaterialFeature::DepthBuffer>;

class MaterialClass {
public:
  MaterialClass(Pipeline, MaterialFeatures);

  [[nodiscard]]
  auto pipeline() const noexcept -> PipelineHandle;

  [[nodiscard]]
  auto features() const noexcept -> MaterialFeatures;

private:
  Pipeline mPipeline;
  MaterialFeatures mFeatures;
};

} // namespace basalt::gfx
