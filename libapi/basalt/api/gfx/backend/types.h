#pragma once

#include <basalt/api/shared/color.h>
#include <basalt/api/shared/handle.h>

#include <basalt/api/math/vector3.h>

#include <basalt/api/base/enum_set.h>
#include <basalt/api/base/types.h>

#include <gsl/span>

#include <memory>
#include <variant>

namespace basalt::gfx {

enum class Attachment : u8 {
  RenderTarget,
  DepthBuffer,
  StencilBuffer,
};
using Attachments = EnumSet<Attachment, Attachment::StencilBuffer>;

enum class BorderColor : u8 {
  BlackTransparent,
  BlackOpaque,
  WhiteOpaque,

  // DeviceCaps.samplerCustomBorderColor
  Custom,
};
constexpr u8 BORDER_COLOR_COUNT {4u};

enum class CullMode : u8 {
  None,
  Clockwise,
  CounterClockwise,
};
constexpr u8 CULL_MODE_COUNT {3u};

enum class TestOp : u8 {
  PassNever,
  PassIfEqual,
  PassIfNotEqual,
  PassIfLess,
  PassIfLessEqual,
  PassIfGreater,
  PassIfGreaterEqual,
  PassAlways,
};
constexpr u8 DEPTH_TEST_PASS_COUNT {8u};

enum class FillMode : u8 {
  Point,
  Wireframe,
  Solid,
};
constexpr u8 FILL_MODE_COUNT {3};

enum class PrimitiveType : u8 {
  PointList,
  LineList,
  LineStrip,
  TriangleList,
  TriangleStrip,
  TriangleFan,
};
constexpr u8 PRIMITIVE_TYPE_COUNT {6u};

enum class ShadeMode : u8 {
  Flat,
  Gouraud,
};
constexpr u8 SHADE_MODE_COUNT {2};

enum class TextureAddressMode : u8 {
  Repeat,
  Mirror,
  ClampToEdge,

  // DeviceCaps.samplerClampToBorder
  ClampToBorder,
  // DeviceCaps.samplerMirrorOnceClampToEdge
  MirrorOnceClampToEdge,
};
constexpr u8 TEXTURE_ADDRESS_MODE_COUNT {5u};

enum class TextureCoordinateSource : u8 {
  Vertex,
  VertexPositionInView,
};
constexpr uSize TEXTURE_COORDINATE_SOURCE_COUNT {2u};

enum class TextureFilter : u8 {
  Point,
  Bilinear,

  // DeviceCaps.samplerMinFilterAnisotropic
  // DeviceCaps.samplerMagFilterAnisotropic
  Anisotropic,
};
constexpr u8 TEXTURE_FILTER_COUNT {3u};

enum class TextureMipFilter : u8 {
  None,
  Point,
  Linear,
};
constexpr u8 TEXTURE_MIP_FILTER_COUNT {3u};

enum class TextureOp : u8 {
  SelectArg1,
  SelectArg2,
  Modulate,
};
constexpr uSize TEXTURE_OP_COUNT {3u};

enum class TextureStageArgument : u8 {
  Diffuse,
  SampledTexture,
};
constexpr u8 TEXTURE_STAGE_ARGUMENT_COUNT {2};

enum class TextureTransformMode : u8 {
  Disabled,
  Count4,
};
constexpr uSize TEXTURE_TRANSFORM_MODE_COUNT {2u};

enum class TransformState : u8 {
  ViewToViewport,
  WorldToView,
  ModelToWorld,
  Texture,
};
constexpr u8 TRANSFORM_STATE_COUNT {4u};

enum class VertexElement : u8 {
  Position3F32,
  PositionTransformed4F32,
  Normal3F32,
  PointSize1F32,
  ColorDiffuse1U32A8R8G8B8,
  ColorSpecular1U32A8R8G8B8,
  TextureCoords1F32,
  TextureCoords2F32,
  TextureCoords3F32,
  TextureCoords4F32,
};
using VertexLayout = gsl::span<const VertexElement>;

enum class IndexType : u8 {
  U16,
  U32,
};
constexpr u8 INDEX_TYPE_COUNT {2u};

using IndexTypes = EnumSet<IndexType, IndexType::U32>;

namespace detail {
struct PipelineTag;
struct SamplerTag;
struct TextureTag;
struct VertexBufferTag;
struct IndexBufferTag;
} // namespace detail
using Pipeline = Handle<detail::PipelineTag>;
using Sampler = Handle<detail::SamplerTag>;
using Texture = Handle<detail::TextureTag>;
using VertexBuffer = Handle<detail::VertexBufferTag>;
using IndexBuffer = Handle<detail::IndexBufferTag>;

struct Command;
class CommandList;

class Device;
using DevicePtr = std::shared_ptr<Device>;

using uDeviceSize = u64;

struct DeviceCaps final {
  uDeviceSize maxVertexBufferSizeInBytes {};
  uDeviceSize maxIndexBufferSizeInBytes {};
  IndexTypes supportedIndexTypes {IndexType::U16};
  u32 maxLights {1};
  u32 maxTextureBlendStages {1};
  u32 maxBoundSampledTextures {1};
  bool samplerClampToBorder {false};
  bool samplerCustomBorderColor {false};
  bool samplerMirrorOnceClampToEdge {false};
  bool samplerMinFilterAnisotropic {false};
  bool samplerMagFilterAnisotropic {false};
  u8 samplerMaxAnisotropy {1};
};

struct TextureBlendingStage final {
  TextureCoordinateSource texCoordinateSrc {TextureCoordinateSource::Vertex};
  TextureTransformMode texCoordinateTransformMode {
    TextureTransformMode::Disabled};
  bool texCoordinateProjected {false};
  TextureStageArgument arg1 {TextureStageArgument::SampledTexture};
  TextureStageArgument arg2 {TextureStageArgument::Diffuse};
  TextureOp colorOp {TextureOp::Modulate};
  TextureOp alphaOp {TextureOp::SelectArg1};
};

struct PipelineDescriptor final {
  VertexLayout vertexInputState;
  gsl::span<const TextureBlendingStage> textureStages {};
  PrimitiveType primitiveType {PrimitiveType::PointList};
  bool lighting {false};
  ShadeMode shadeMode {ShadeMode::Gouraud};
  CullMode cullMode {CullMode::None};
  FillMode fillMode {FillMode::Solid};
  TestOp depthTest {TestOp::PassAlways};
  bool depthWriteEnable {false};
  bool dithering {false};
};

struct SamplerDescriptor final {
  TextureFilter magFilter {TextureFilter::Point};
  TextureFilter minFilter {TextureFilter::Point};
  TextureMipFilter mipFilter {TextureMipFilter::None};
  TextureAddressMode addressModeU {TextureAddressMode::Repeat};
  TextureAddressMode addressModeV {TextureAddressMode::Repeat};
  TextureAddressMode addressModeW {TextureAddressMode::Repeat};

  // DeviceCaps.samplerClampToBorder
  BorderColor borderColor {BorderColor::BlackTransparent};
  // DeviceCaps.samplerCustomBorderColor
  Color customBorderColor;
  // DeviceCaps.samplerMaxAnisotropy
  u8 maxAnisotropy {1};
};

struct VertexBufferDescriptor final {
  uDeviceSize sizeInBytes {};
  VertexLayout layout {};
};

struct IndexBufferDescriptor final {
  uDeviceSize sizeInBytes {};
  IndexType type {IndexType::U16};
};

struct DirectionalLight final {
  Vector3f32 direction;
  Color diffuseColor;
  Color ambientColor;
};

struct PointLight final {
  Color diffuseColor {};
  Color ambientColor {};
  Vector3f32 positionInWorld {0.0f};
  f32 rangeInWorld {};
  f32 attenuation0 {};
  f32 attenuation1 {};
  f32 attenuation2 {};
};

struct SpotLight final {
  Color diffuseColor {};
  Color ambientColor {};
  Vector3f32 positionInWorld {0.0f};
  Vector3f32 directionInWorld {0.0f};
  f32 rangeInWorld {};
  f32 attenuation0 {};
  f32 attenuation1 {};
  f32 attenuation2 {};
  f32 falloff {};
  f32 phi {};
  f32 theta {};
};

using Light = std::variant<PointLight, SpotLight, DirectionalLight>;

} // namespace basalt::gfx
