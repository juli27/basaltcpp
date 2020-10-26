#pragma once

#include "backend/types.h"

#include "api/math/mat4.h"

#include "api/shared/color.h"
#include "api/shared/handle.h"

#include "api/base/enum_array.h"
#include "api/base/types.h"

#include <gsl/span>

#include <cstddef>
#include <memory>

namespace basalt::gfx {

struct DrawTarget;

struct Drawable;
using DrawablePtr = std::shared_ptr<Drawable>;

struct ResourceCache;

namespace detail {

struct MeshTag;
struct MaterialTag;

} // namespace detail

using Mesh = Handle<detail::MeshTag>;
using Material = Handle<detail::MaterialTag>;

// using Texture = std::string;
// struct VertexColor final {};
// using Diffuse = std::variant<Color, VertexColor, Texture>;

enum class TextureCoordinateSource : u8 { Vertex, VertexPositionCameraSpace };
enum class TextureTransformMode : u8 { Disabled, Count4 };

struct MeshDescriptor final {
  gsl::span<const std::byte> data;
  VertexLayout layout;
  PrimitiveType primitiveType {PrimitiveType::PointList};
  u32 primitiveCount {};
};

static_assert(sizeof(MeshDescriptor) == 56);

struct MaterialDescriptor final {
  Color diffuse;
  Color ambient;

  bool cullBackFace {true};
  bool lit {true};

  TextureCoordinateSource textureCoordinateSource {
    TextureCoordinateSource::Vertex};
  TextureTransformMode textureTransformMode {TextureTransformMode::Disabled};
  bool textureTransformProjected {false};
};

static_assert(sizeof(MaterialDescriptor) == 40);

struct RenderComponent final {
  Mesh mesh {Mesh::null()};
  Texture texture {Texture::null()};
  Material material {Material::null()};
  Mat4f32 texTransform {Mat4f32::identity()};
};

static_assert(sizeof(RenderComponent) == 76);

struct MeshData final {
  VertexBuffer vertexBuffer {VertexBuffer::null()};
  PrimitiveType primitiveType {PrimitiveType::PointList};
  u32 startVertex {};
  u32 primitiveCount {};
};

struct MaterialData final {
  EnumArray<RenderState, u32, RENDER_STATE_COUNT> renderStates {};
  EnumArray<TextureStageState, u32, TEXTURE_STAGE_STATE_COUNT>
    textureStageStates {};

  Color diffuse;
  Color ambient;
};

} // namespace basalt::gfx
