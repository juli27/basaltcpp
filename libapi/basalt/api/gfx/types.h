#pragma once

#include <basalt/api/gfx/backend/types.h>
#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/api/math/mat4.h>

#include <basalt/api/shared/color.h>
#include <basalt/api/shared/handle.h>

#include <basalt/api/base/enum_array.h>
#include <basalt/api/base/types.h>

#include <gsl/span>

#include <cstddef>
#include <vector>

namespace basalt::gfx {

struct ResourceCache;

namespace detail {

struct MeshTag;
struct MaterialTag;
struct MaterialTemplateTag;

} // namespace detail

using Mesh = Handle<detail::MeshTag>;
using Material = Handle<detail::MaterialTag>;
using MaterialTemplate = Handle<detail::MaterialTemplateTag>;

struct MeshDescriptor final {
  gsl::span<const std::byte> vertexData;
  u32 vertexCount {};
  VertexLayout layout;
};
#if _DEBUG
static_assert(sizeof(MeshDescriptor) == 56);
#else
static_assert(sizeof(MeshDescriptor) == 48);
#endif

struct SampledTextureDescriptor final {
  Texture texture {Texture::null()};
  TextureFilter filter {TextureFilter::Point};
  TextureMipFilter mipFilter {TextureMipFilter::None};
  TextureAddressMode addressModeU {TextureAddressMode::WrapRepeat};
  TextureAddressMode addressModeV {TextureAddressMode::WrapRepeat};
};

struct MaterialDescriptor final {
  MaterialTemplate materialTemplate;

  Color diffuse;
  Color ambient;

  SampledTextureDescriptor sampledTexture;

  bool solid {true};
};

// TODO: Ref-counted and actually destroy them
struct MaterialTemplateDescriptor final {
  PrimitiveType primitiveType {PrimitiveType::TriangleList};
  CullMode cullMode {CullMode::CounterClockwise};
  bool lighting {true};

  TextureCoordinateSource textureCoordinateSource {
    TextureCoordinateSource::Vertex};
  TextureTransformMode textureTransformMode {TextureTransformMode::Disabled};
  bool textureTransformProjected {false};
};

struct RenderComponent final {
  Mesh mesh {Mesh::null()};
  Material material {Material::null()};
};

struct MeshData final {
  VertexBuffer vertexBuffer;
  u32 startVertex {};
  u32 vertexCount {};
};

struct MaterialData final {
  MaterialTemplate materialTemplate;

  using RenderStates =
    EnumArray<RenderStateType, RenderStateValue, RENDER_STATE_COUNT>;

  RenderStates renderStates;
  Mat4f32 texTransform {Mat4f32::identity()};

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
