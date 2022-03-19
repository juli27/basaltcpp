#pragma once

#include <basalt/api/scene/types.h>

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
  Color,
  ZBuffer,
  StencilBuffer,
};
using Attachments = EnumSet<Attachment, Attachment::StencilBuffer>;

enum class CommandType : u8 {
  ClearAttachments,
  Draw,
  SetRenderState,
  BindPipeline,
  BindVertexBuffer,
  BindSampler,
  BindTexture,

  // fixed function only
  SetTransform,
  SetAmbientLight,
  SetLights,
  SetMaterial,
  SetTextureStageState,

  // built-in extensions
  ExtDrawXModel,
  ExtRenderDearImGui,
};

enum class ContextStatus : u8 {
  Ok,
  Error,
  DeviceLost,
  ResetNeeded,
};

enum class CullMode : u8 {
  None,
  Clockwise,
  CounterClockwise,
};
constexpr uSize CULL_MODE_COUNT = 3u;

enum class DepthTestPass : u8 {
  Never,
  IfEqual,
  IfNotEqual,
  IfLess,
  IfLessEqual,
  IfGreater,
  IfGreaterEqual,
  Always,
};
constexpr uSize DEPTH_TEST_PASS_COUNT = 8u;

enum class FillMode : u8 {
  Point,
  Wireframe,
  Solid,
};
constexpr uSize FILL_MODE_COUNT = 3u;

enum class PresentResult : u8 {
  Ok,
  DeviceLost,
};

enum class PrimitiveType : u8 {
  PointList,
  LineList,
  LineStrip,
  TriangleList,
  TriangleStrip,
  TriangleFan,
};
constexpr uSize PRIMITIVE_TYPE_COUNT = 6u;

enum class RenderStateType : u8 {
  // pixel state
  FillMode,
  // - fixed function only
  ShadeMode,
};
constexpr uSize RENDER_STATE_COUNT = 2u;

enum class ShadeMode : u8 {
  Flat,
  Gouraud,
};
constexpr uSize SHADE_MODE_COUNT = 2u;

// TODO: support border and custom border color (no/partial support in Vulkan
// and OpenGL ES)
// TODO: support MirrorOnce
enum class TextureAddressMode : u8 {
  WrapRepeat,
  MirrorRepeat,
  ClampEdge,
};
constexpr uSize TEXTURE_ADDRESS_MODE_COUNT {3u};

enum class TextureCoordinateSource : u8 {
  Vertex,
  VertexPositionInView,
};
constexpr uSize TEXTURE_COORDINATE_SOURCE_COUNT {2u};

enum class TextureFilter : u8 {
  Point,
  Linear,
  LinearAnisotropic, // TODO: check for support and set level
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

enum class TextureStageState : u8 {
  TextureTransformFlags,
};
constexpr uSize TEXTURE_STAGE_STATE_COUNT = 1u;

enum TextureTransformFlags : u8 {
  TtfDisabled = 0,
  TtfCount4 = 1,

  TtfProjected = 0x2,
};

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
struct CommandSetRenderState;
struct CommandBindPipeline;
struct CommandBindVertexBuffer;
struct CommandBindSampler;
struct CommandBindTexture;
struct CommandSetTransform;
struct CommandSetAmbientLight;
struct CommandSetLights;
struct CommandSetMaterial;
struct CommandSetTextureStageState;

struct CommandList;
using Composite = std::vector<CommandList>;

struct Context;
using ContextPtr = std::unique_ptr<Context>;

struct Device;

struct RenderState;
using RenderStateValue = std::variant<FillMode, ShadeMode>;

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
};

struct TextureBlendingStage final {
  TextureStageArgument arg1 {TextureStageArgument::SampledTexture};
  TextureStageArgument arg2 {TextureStageArgument::Diffuse};
  TextureOp colorOp {TextureOp::Modulate};
  TextureOp alphaOp {TextureOp::SelectArg1};
  TextureCoordinateSource texCoordinateSrc {TextureCoordinateSource::Vertex};
};

struct PipelineDescriptor final {
  gsl::span<const TextureBlendingStage> textureStages {};
  PrimitiveType primitiveType {PrimitiveType::PointList};
  bool lighting {false};
  CullMode cullMode {CullMode::None};
  DepthTestPass depthTest {DepthTestPass::Always};
  bool depthWriteEnable {false};
};

struct SamplerDescriptor final {
  TextureFilter filter {TextureFilter::Point};
  TextureMipFilter mipFilter {TextureMipFilter::None};
  TextureAddressMode addressModeU {TextureAddressMode::WrapRepeat};
  TextureAddressMode addressModeV {TextureAddressMode::WrapRepeat};
  TextureAddressMode addressModeW {TextureAddressMode::WrapRepeat};
};

struct VertexBufferDescriptor final {
  uDeviceSize sizeInBytes {};
  VertexLayout layout {};
};

struct PointLight final {
  Color diffuseColor {};
  Color ambientColor {};
  Vector3f32 positionInWorld {};
  f32 rangeInWorld {};
  f32 attenuation0 {};
  f32 attenuation1 {};
  f32 attenuation2 {};
};

struct SpotLight final {
  Color diffuseColor {};
  Color ambientColor {};
  Vector3f32 positionInWorld {};
  Vector3f32 directionInWorld {};
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
