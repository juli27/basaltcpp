#pragma once

#include "backend/types.h"
#include "backend/ext/types.h"

#include "basalt/api/math/angle.h"

#include "basalt/api/shared/color.h"
#include "basalt/api/shared/handle.h"
#include "basalt/api/shared/types.h"

#include "basalt/api/base/enum_set.h"
#include "basalt/api/base/types.h"

#include <gsl/span>

#include <cstddef>
#include <memory>
#include <variant>

namespace basalt::gfx {

struct Camera;

class Context;
using ContextPtr = std::shared_ptr<Context>;

class ContextResourceDeleter;

class Environment;
class GfxSystem;

struct Info;

class ResourceCache;
using ResourceCachePtr = std::shared_ptr<ResourceCache>;

using Pipeline = UniqueHandle<PipelineHandle, ContextResourceDeleter>;
using Sampler = UniqueHandle<SamplerHandle, ContextResourceDeleter>;
using Texture = UniqueHandle<TextureHandle, ContextResourceDeleter>;
using VertexBuffer = UniqueHandle<VertexBufferHandle, ContextResourceDeleter>;
using IndexBuffer = UniqueHandle<IndexBufferHandle, ContextResourceDeleter>;

BASALT_DEFINE_HANDLE(MeshHandle);
using Mesh = UniqueHandle<MeshHandle, ContextResourceDeleter>;

BASALT_DEFINE_HANDLE(MaterialHandle);
using Material = UniqueHandle<MaterialHandle, ContextResourceDeleter>;

enum class BackendApi : u8 {
  Default,
  Direct3D9,
};
constexpr auto BACKEND_API_COUNT = u8{2};

struct MeshCreateInfo final {
  gsl::span<std::byte const> vertexData;
  u32 vertexCount{};
  VertexLayoutSpan layout;
  gsl::span<std::byte const> indexData;
  u32 indexCount{};
  IndexType indexType{IndexType::U16};
};

struct SampledTexture final {
  TextureHandle texture;
  SamplerHandle sampler;
};

struct MaterialCreateInfo final {
  PipelineHandle pipeline;
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

struct Model {
  MeshHandle mesh;
  MaterialHandle material;
};

using DirectionalLight = DirectionalLightData;

struct PointLight {
  Color diffuse;
  Color specular;
  Color ambient;
  f32 range{};
  f32 attenuation0{};
  f32 attenuation1{};
  f32 attenuation2{};
};

struct SpotLight {
  Color diffuse;
  Color specular;
  Color ambient;
  Vector3f32 direction;
  f32 range{};
  f32 attenuation0{};
  f32 attenuation1{};
  f32 attenuation2{};
  f32 falloff{};
  Angle phi;
  Angle theta;
};

using Light = std::variant<PointLight, SpotLight>;

struct MeshData final {
  VertexBufferHandle vertexBuffer;
  u32 startVertex;
  u32 vertexCount;
  IndexBufferHandle indexBuffer{};
  u32 indexCount{};
};

struct MaterialData final {
  Color diffuse;
  Color ambient;
  Color emissive;
  Color specular;
  f32 specularPower{};
  Color fogColor;
  f32 fogStart{};
  f32 fogEnd{};
  f32 fogDensity{};

  PipelineHandle pipeline;
  TextureHandle texture{};
  SamplerHandle sampler{};
};

namespace ext {

struct XModel {
  MaterialHandle material;
  XMeshHandle mesh;
};

} // namespace ext

} // namespace basalt::gfx
