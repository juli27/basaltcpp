#include <basalt/api/gfx/material_class.h>

#include <utility>

namespace basalt::gfx {

MaterialClass::MaterialClass(Pipeline pipeline,
                             MaterialFeatures const features)
  : mPipeline{std::move(pipeline)}
  , mFeatures{features} {
}

auto MaterialClass::pipeline() const noexcept -> PipelineHandle {
  return mPipeline.handle();
}

auto MaterialClass::features() const noexcept -> MaterialFeatures {
  return mFeatures;
}

} // namespace basalt::gfx
