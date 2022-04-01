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

} // namespace detail

using Mesh = Handle<detail::MeshTag>;
using Material = Handle<detail::MaterialTag>;

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
  Color diffuse;
  Color ambient;

  SampledTextureDescriptor sampledTexture;

  PrimitiveType primitiveType {PrimitiveType::PointList};
  // TODO: upgrade boolean flag to CullMode enum
  bool cullBackFace {true};
  bool lit {true};
  bool solid {true};

  TextureCoordinateSource textureCoordinateSource {
    TextureCoordinateSource::Vertex};
  TextureTransformMode textureTransformMode {TextureTransformMode::Disabled};
  bool textureTransformProjected {false};
};

static_assert(sizeof(MaterialDescriptor) == 48);

struct RenderComponent final {
  Mesh mesh {Mesh::null()};
  Material material {Material::null()};
  Mat4f32 texTransform {Mat4f32::identity()};
};

static_assert(sizeof(RenderComponent) == 72);

struct MeshData final {
  VertexBuffer vertexBuffer;
  u32 startVertex {};
  u32 vertexCount {};
};

struct MaterialData final {
  using RenderStates =
    EnumArray<RenderStateType, RenderStateValue, RENDER_STATE_COUNT>;

  RenderStates renderStates;

  Color diffuse;
  Color ambient;

  Pipeline pipeline;
  Texture texture;
  Sampler sampler;
};

static_assert(sizeof(MaterialData) == 48);

struct XModelData final {
  std::vector<Material> materials;
  ext::XMesh mesh;
};

} // namespace basalt::gfx
