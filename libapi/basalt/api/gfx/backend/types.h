#pragma once

#include <basalt/api/shared/asserts.h>
#include <basalt/api/shared/color.h>
#include <basalt/api/shared/handle.h>

#include <basalt/api/math/vector3.h>

#include <basalt/api/base/enum_set.h>
#include <basalt/api/base/types.h>

#include <gsl/span>

#include <memory>
#include <variant>
#include <vector>

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

enum class CommandType : u8 {
  ClearAttachments,
  Draw,
  BindPipeline,
  BindVertexBuffer,
  BindSampler,
  BindTexture,

  // fixed function only
  SetTransform,
  SetAmbientLight,
  SetLights,
  SetMaterial,

  // built-in extensions
  ExtDrawXMesh,
  ExtRenderDearImGui,
};

enum class CullMode : u8 {
  None,
  Clockwise,
  CounterClockwise,
};
constexpr uSize CULL_MODE_COUNT = 3u;

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
constexpr uSize DEPTH_TEST_PASS_COUNT = 8u;

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
constexpr uSize PRIMITIVE_TYPE_COUNT = 6u;

enum class ShadeMode : u8 {
  Flat,
  Gouraud,
};
constexpr u8 SHADE_MODE_COUNT {2};

// TODO: support border and custom border color (no/partial support in Vulkan
// and OpenGL ES)
// TODO: support MirrorOnce
enum class TextureAddressMode : u8 {
  Repeat,
  Mirror,
  ClampToEdge,

  // DeviceCaps.samplerClampToBorder
  ClampToBorder,
};
constexpr uSize TEXTURE_ADDRESS_MODE_COUNT {4u};

enum class TextureCoordinateSource : u8 {
  Vertex,
  VertexPositionInView,
};
constexpr uSize TEXTURE_COORDINATE_SOURCE_COUNT {2u};

enum class TextureFilter : u8 {
  Point,
  Bilinear,
  Anisotropic, // TODO: check for support and set level
};
constexpr uSize TEXTURE_FILTER_COUNT {3u};

enum class TextureMipFilter : u8 {
  None,
  Point,
  Linear,
};
constexpr uSize TEXTURE_MIP_FILTER_COUNT {3u};

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
constexpr uSize TRANSFORM_STATE_COUNT = 4u;

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
using VertexLayout = std::vector<VertexElement>;

namespace detail {
struct PipelineTag;
struct SamplerTag;
struct TextureTag;
struct VertexBufferTag;
} // namespace detail
using Pipeline = Handle<detail::PipelineTag>;
using Sampler = Handle<detail::SamplerTag>;
using Texture = Handle<detail::TextureTag>;
using VertexBuffer = Handle<detail::VertexBufferTag>;

struct Command;
struct CommandClearAttachments;
struct CommandDraw;
struct CommandBindPipeline;
struct CommandBindVertexBuffer;
struct CommandBindSampler;
struct CommandBindTexture;
struct CommandSetTransform;
struct CommandSetAmbientLight;
struct CommandSetLights;
struct CommandSetMaterial;

struct CommandList;
struct Device;

struct Command {
  const CommandType type;

  template <typename T>
  auto as() const -> const T& {
    BASALT_ASSERT(type == T::TYPE, "invalid command cast");
    return *static_cast<const T*>(this);
  }

protected:
  constexpr explicit Command(const CommandType t) noexcept : type {t} {
  }
};

using CommandPtr = std::unique_ptr<Command>;

template <CommandType Type>
struct CommandT : Command {
  static constexpr CommandType TYPE {Type};

  constexpr CommandT() noexcept : Command {TYPE} {
  }
};

using uDeviceSize = u64;

struct DeviceCaps final {
  uDeviceSize maxVertexBufferSizeInBytes {};
  u32 maxLights {1};
  u32 maxTextureBlendStages {1};
  u32 maxBoundSampledTextures {1};
  u32 maxTextureAnisotropy {1};
  bool samplerClampToBorder {false};
  bool samplerCustomBorderColor {false};
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
  gsl::span<const VertexElement> vertexInputState;
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
};

struct VertexBufferDescriptor final {
  uDeviceSize sizeInBytes {};
  VertexLayout layout {};
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
