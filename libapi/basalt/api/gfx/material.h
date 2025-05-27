#pragma once

#include "backend/types.h"

#include <basalt/api/shared/color.h>

#include <basalt/api/math/matrix4x4.h>

#include <basalt/api/base/types.h>

namespace basalt::gfx {

struct SampledTexture {
  TextureHandle texture;
  SamplerHandle sampler;
};

struct MaterialCreateInfo {
  MaterialClassHandle clazz;

  Matrix4x4f32 texTransform; // TODO: this is temporary
  Color diffuse;
  Color ambient;
  Color emissive;
  Color specular;
  f32 specularPower{};
  SampledTexture sampledTexture;
  Color fogColor;
  f32 fogStart{};
  f32 fogEnd{};
  f32 fogDensity{};
};

struct MaterialData {
  MaterialClassHandle clazz;

  Matrix4x4f32 texTransform; // TODO: temp
  Color diffuse;
  Color ambient;
  Color emissive;
  Color specular;
  f32 specularPower{};
  Color fogColor;
  f32 fogStart{};
  f32 fogEnd{};
  f32 fogDensity{};

  TextureHandle texture{};
  SamplerHandle sampler{};
};

} // namespace basalt::gfx
