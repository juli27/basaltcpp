#include <basalt/api/gfx/material.h>

#include <algorithm>
#include <memory>
#include <utility>

namespace basalt::gfx {

Material::Material(MaterialClassHandle const clazz,
                   PipelineHandle const pipeline,
                   MaterialFeatures const features,
                   std::vector<MaterialProperty> properties)
  : mClass{clazz}
  , mPipeline{pipeline}
  , mFeatures{features}
  , mProperties{std::move(properties)} {
}

auto Material::features() const -> MaterialFeatures {
  return mFeatures;
}

auto Material::properties() const -> gsl::span<MaterialProperty const> {
  return mProperties;
}

auto Material::clazz() const -> MaterialClassHandle {
  return mClass;
}

auto Material::pipeline() const -> PipelineHandle {
  return mPipeline;
}

auto Material::get_value(MaterialPropertyId const id) const
  -> std::optional<MaterialPropertyValue> {
  auto const property = find_property(id);
  if (!property) {
    return std::nullopt;
  }

  return (*property)->value;
}

auto Material::set_value(MaterialPropertyId const id,
                         MaterialPropertyValue const value) -> void {
  auto const property = find_property(id);
  BASALT_ASSERT(property);
  if (!property) {
    return;
  }

  (*property)->value = value;
}

auto Material::find_property(MaterialPropertyId const id) const
  -> std::optional<MaterialProperty const*> {
  auto const it =
    std::find_if(mProperties.cbegin(), mProperties.cend(),
                 [&](MaterialProperty const& p) { return p.id == id; });
  if (it == mProperties.cend()) {
    return std::nullopt;
  }

  return std::addressof(*it);
}

auto Material::find_property(MaterialPropertyId const id)
  -> std::optional<MaterialProperty*> {
  auto const it =
    std::find_if(mProperties.begin(), mProperties.end(),
                 [&](MaterialProperty const& p) { return p.id == id; });
  if (it == mProperties.cend()) {
    return std::nullopt;
  }

  return std::addressof(*it);
}

} // namespace basalt::gfx
