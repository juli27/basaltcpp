#pragma once

#include "backend/types.h"
#include "backend/ext/types.h"

#include "basalt/api/math/angle.h"

#include "basalt/api/shared/color.h"
#include "basalt/api/shared/handle.h"

#include "basalt/api/base/enum_set.h"
#include "basalt/api/base/types.h"

#include <gsl/span>

#include <cstddef>
#include <filesystem>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace basalt::gfx {

struct Camera;

class Context;
using ContextPtr = std::shared_ptr<Context>;

class Environment;
class GfxSystem;

class ResourceCache;
using ResourceCachePtr = std::shared_ptr<ResourceCache>;

namespace detail {

struct MeshTag;
struct MaterialTag;
struct AdapterTag;

} // namespace detail

using MeshHandle = Handle<detail::MeshTag>;
using MaterialHandle = Handle<detail::MaterialTag>;

using Adapter = Handle<detail::AdapterTag>;

// listed from left to right, in c-array order if power of 2 aligned,
// lsb to msb otherwise
enum class ImageFormat : u8 {
  Unknown,

  // Color
  // 16-bit
  B5G6R5,
  B5G5R5X1,
  B5G5R5A1,

  // 32-bit
  B8G8R8X8,
  B8G8R8A8,
  B10G10R10A2,

  // signed data
  // 16-bit
  U8V8,

  // Depth-Stencil
  // 16-bit
  D16,

  // 32-bit
  D24X8,
  D24S8,
};
constexpr auto IMAGE_FORMAT_COUNT = u8{11};

enum class BackendApi : u8 {
  Default,
  Direct3D9,
};
constexpr auto BACKEND_API_COUNT = u8{2};

enum class MultiSampleCount : u8 {
  One,
  Two,
  Four,
  Eight,
};
constexpr auto MULTI_SAMPLE_COUNT_COUNT = u8{4};

using MultiSampleCounts = EnumSet<MultiSampleCount, MultiSampleCount::Eight>;

struct BackBufferFormat final {
  ImageFormat renderTargetFormat{ImageFormat::Unknown};
  ImageFormat depthStencilFormat{ImageFormat::Unknown};
  MultiSampleCounts supportedSampleCounts;
};

struct DisplayMode final {
  u32 width{};
  u32 height{};
  u32 refreshRate{};
};

using DisplayModeList = std::vector<DisplayMode>;

struct AdapterModes final {
  std::vector<BackBufferFormat> backBufferFormats;
  DisplayModeList displayModes;
  ImageFormat displayFormat{ImageFormat::Unknown};
};

using AdapterModeList = std::vector<AdapterModes>;

struct AdapterInfo final {
  std::string displayName{};
  std::string driverInfo{};
  AdapterModeList adapterModes{};
  std::vector<BackBufferFormat> backBufferFormats;
  DisplayMode displayMode;
  ImageFormat displayFormat{ImageFormat::Unknown};
  Adapter handle;
};

using AdapterList = std::vector<AdapterInfo>;

struct Info final {
  // HACK: caps for the current device only
  DeviceCaps currentDeviceCaps;
  AdapterList adapters{};
  BackendApi backendApi{BackendApi::Default};
};

struct MeshCreateInfo final {
  gsl::span<std::byte const> vertexData;
  u32 vertexCount{};
  VertexLayoutSpan layout;
  gsl::span<std::byte const> indexData;
  u32 indexCount{};
  IndexType indexType{IndexType::U16};
};

struct SampledTexture final {
  TextureHandle texture = TextureHandle::null();
  SamplerHandle sampler = SamplerHandle::null();
};

struct MaterialCreateInfo final {
  PipelineHandle pipeline = PipelineHandle::null();
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

struct RenderComponent final {
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

struct XModelLoadInfo final {
  std::filesystem::path filePath;

  // optional materials to override (in order of the model)
  gsl::span<MaterialHandle> materials{};
};

struct XModelData final {
  // indices imply a 1:1 mapping between mesh and material
  std::vector<ext::XMeshHandle> meshes;
  std::vector<MaterialHandle> materials;
};

namespace ext {

struct XModelCreateInfo {
  gsl::span<XMeshHandle> meshes;
  gsl::span<MaterialHandle> materials;
};

} // namespace ext

} // namespace basalt::gfx
