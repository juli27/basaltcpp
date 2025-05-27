#pragma once

#include "material_class.h" // IWYU pragma: export

#include "types.h"
#include "backend/types.h"

#include <basalt/api/shared/color.h>

#include <basalt/api/math/matrix4x4.h>

#include <basalt/api/base/types.h>

#include <gsl/span>

#include <optional>
#include <variant>
#include <vector>

namespace basalt::gfx {

struct UniformColors {
  Color diffuse{};
  Color ambient{};
  Color emissive{};
  Color specular{};
  f32 specularPower{};
};

struct FogParameters {
  Color color{};
  f32 start{};
  f32 end{};
  f32 density{};
};

struct SampledTexture {
  SamplerHandle sampler;
  TextureHandle texture;
};

using MaterialPropertyValue =
  std::variant<UniformColors, FogParameters, SampledTexture, Matrix4x4f32>;

struct MaterialProperty {
  MaterialPropertyId id;
  MaterialPropertyValue value;
};

struct MaterialCreateInfo {
  MaterialClassHandle clazz;
  gsl::span<MaterialProperty const> initialValues;
};

class Material {
public:
  Material(MaterialClassHandle, PipelineHandle, MaterialFeatures,
           std::vector<MaterialProperty>);

  [[nodiscard]]
  auto features() const -> MaterialFeatures;
  [[nodiscard]]
  auto properties() const -> gsl::span<MaterialProperty const>;
  [[nodiscard]]
  auto clazz() const -> MaterialClassHandle;
  [[nodiscard]]
  auto pipeline() const -> PipelineHandle;

  [[nodiscard]]
  auto get_value(MaterialPropertyId) const -> std::optional<MaterialPropertyValue>;
  auto set_value(MaterialPropertyId, MaterialPropertyValue) -> void;

private:
  MaterialClassHandle mClass;
  PipelineHandle mPipeline;
  MaterialFeatures mFeatures;
  // FIXME: reduce size
  std::vector<MaterialProperty> mProperties;

  auto find_property(MaterialPropertyId) const -> std::optional<MaterialProperty const*>;
  auto find_property(MaterialPropertyId) -> std::optional<MaterialProperty*>;
};

} // namespace basalt::gfx
