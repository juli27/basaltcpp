#pragma once

#include <basalt/api/gfx/backend/types.h>
#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/api/math/matrix4x4.h>

#include <basalt/api/shared/color.h>
#include <basalt/api/shared/handle.h>

#include <basalt/api/base/enum_set.h>
#include <basalt/api/base/types.h>

#include <gsl/span>

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace basalt::gfx {

class GfxSystem;

class ResourceCache;
using ResourceCachePtr = std::shared_ptr<ResourceCache>;

namespace detail {

struct MeshTag;
struct MaterialTag;

} // namespace detail

using Mesh = Handle<detail::MeshTag>;
using Material = Handle<detail::MaterialTag>;

namespace detail {

struct AdapterTag;

}

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

  // Depth-Stencil
  // 16-bit
  D16,

  // 32-bit
  D24X8,
  D24S8,
};
constexpr u8 IMAGE_FORMAT_COUNT {10};

// SERIALIZED
enum class BackendApi : u8 {
  Default = 0,
  Direct3D9 = 1,
};
constexpr uSize BACKEND_API_COUNT {2u};

enum class MultiSampleCount : u8 {
  One,
  Two,
  Four,
  Eight,
};
constexpr u8 MULTI_SAMPLE_COUNT_COUNT {4};

using MultiSampleCounts = EnumSet<MultiSampleCount, MultiSampleCount::Eight>;

struct BackBufferFormat final {
  ImageFormat renderTargetFormat {ImageFormat::Unknown};
  ImageFormat depthStencilFormat {ImageFormat::Unknown};
  MultiSampleCounts supportedSampleCounts;
};

struct DisplayMode final {
  u32 width {};
  u32 height {};
  u32 refreshRate {};
};

using DisplayModeList = std::vector<DisplayMode>;

struct AdapterModes final {
  std::vector<BackBufferFormat> backBufferFormats;
  DisplayModeList displayModes;
  ImageFormat displayFormat {ImageFormat::Unknown};
};

using AdapterModeList = std::vector<AdapterModes>;

struct AdapterInfo final {
  std::string displayName {};
  std::string driverInfo {};
  AdapterModeList adapterModes {};
  std::vector<BackBufferFormat> backBufferFormats;
  DisplayMode displayMode;
  ImageFormat displayFormat {ImageFormat::Unknown};
  Adapter handle;
};

using AdapterList = std::vector<AdapterInfo>;

struct Info final {
  // HACK: caps for the current device only
  DeviceCaps currentDeviceCaps;
  AdapterList adapters {};
  BackendApi backendApi {BackendApi::Default};
};

struct MeshDescriptor final {
  gsl::span<const std::byte> vertexData;
  u32 vertexCount {};
  VertexLayout layout;
  gsl::span<const std::byte> indexData;
  u32 indexCount {};
  IndexType indexType {IndexType::U16};
};

struct SampledTextureDescriptor final {
  Texture texture {Texture::null()};
  TextureFilter filter {TextureFilter::Point};
  TextureMipFilter mipFilter {TextureMipFilter::None};
  TextureAddressMode addressModeU {TextureAddressMode::Repeat};
  TextureAddressMode addressModeV {TextureAddressMode::Repeat};
};

struct MaterialDescriptor final {
  PipelineDescriptor* pipelineDesc {};
  Color diffuse;
  Color ambient;

  SampledTextureDescriptor sampledTexture;
};

struct RenderComponent final {
  Mesh mesh {Mesh::null()};
  Material material {Material::null()};
  Matrix4x4f32 texTransform {Matrix4x4f32::identity()};
};

static_assert(sizeof(RenderComponent) == 72);

struct MeshData final {
  VertexBuffer vertexBuffer;
  u32 startVertex {};
  u32 vertexCount {};
  IndexBuffer indexBuffer;
  u32 indexCount {};
};

struct MaterialData final {
  Color diffuse;
  Color ambient;

  Pipeline pipeline;
  Texture texture;
  Sampler sampler;
};

struct XModelData final {
  std::vector<Material> materials;
  ext::XMesh mesh;
};

} // namespace basalt::gfx
