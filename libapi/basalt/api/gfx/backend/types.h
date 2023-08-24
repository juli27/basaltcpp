#pragma once

#include <basalt/api/shared/color.h>
#include <basalt/api/shared/handle.h>

#include <basalt/api/math/angle.h>
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

enum class BlendFactor : u8 {
  Zero,
  One,
  SrcColor,
  OneMinusSrcColor,
  SrcAlpha,
  OneMinusSrcAlpha,
  Constant,
  OneMinusConstant,
};
constexpr u8 BLEND_FACTOR_COUNT {8};

enum class BlendOp : u8 {
  Add,
  Subtract,
  ReverseSubtract,
};
constexpr u8 BLEND_OP_COUNT {3};

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

enum class TestPassCond : u8 {
  Never,
  IfEqual,
  IfNotEqual,
  IfLess,
  IfLessEqual,
  IfGreater,
  IfGreaterEqual,
  Always,
};
constexpr u8 TEST_PASS_COND_COUNT {8u};

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

enum class FogMode : u8 {
  None,
  Linear,
  Exponential,
  ExponentialSquared,
};
constexpr u8 FOG_MODE_COUNT {4};

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

enum class TextureCoordinateSrc : u8 {
  Vertex,
  PositionInViewSpace,
  NormalInViewSpace,
  ReflectionVectorInViewSpace,
};
constexpr u8 TEXTURE_COORDINATE_SOURCE_COUNT {4};

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
  // r = arg1
  Replace,
  // r = arg1 * arg2
  Modulate,
  // r = 2 * arg1 * arg2
  Modulate2X,
  // r = 4 * arg1 * arg2
  Modulate4X,
  // r = arg1 + arg2
  Add,
  // r = arg1 + arg2 - 0.5
  AddSigned,
  // r = 2 * (arg1 + arg2 - 0.5)
  AddSigned2X,
  // r = arg1 - arg2
  Subtract,
  // r = arg1 + arg2 - arg1 * arg2 = arg1 + arg2 (1 - arg1)
  AddSmooth,
  // r = arg1 * alpha_d + arg2 * (1 - alpha_d)
  BlendDiffuseAlpha,
  // r = arg1 * alpha_t + arg2 * (1 - alpha_t)
  BlendTextureAlpha,
  // r = arg1 * alpha_f + arg2 * (1 - alpha_f)
  BlendFactorAlpha,
  // r = arg1 * alpha_c + arg2 * (1 - alpha_c)
  BlendCurrentAlpha,
  // r = arg1 + arg2 * (1 - alpha_t)
  BlendTextureAlphaPm,
  // r = arg1, current = texture_{n+1} * current, if there is a texture in the
  // next stage
  PreModulate,
  // r.rgba = arg1.rgb + arg1.a * arg2.rgb
  ModulateAlphaAddColor,
  // r.rgba = arg1.rgb * arg2.rgb + arg1.a
  ModulateColorAddAlpha,
  // r.rgba = arg1.rgb + (1 - arg1.a) * arg2.rgb
  ModulateInvAlphaAddColor,
  // r.rgba = (1 - arg1.rgb) * arg2.rgb + arg1.a
  ModulateInvColorAddAlpha,
  // performs per-pixel bump mapping using the env map in the next stage without
  // luminance
  BumpEnvMap,
  // performs per-pixel bump mapping using the env map in the next stage with
  // luminance
  BumpEnvMapLuminance,
  // r.rgba = arg1.r * arg2.r + arg1.g * arg2.g + arg1.b * arg2.b
  // with args biased to be signed
  DotProduct3,
  // r.rgba = arg1 + arg2 * arg3
  MultiplyAdd,
  // r.rgba = arg1 * arg2 + (1 - arg1) * arg3
  Interpolate,
};
constexpr u8 TEXTURE_OP_COUNT {24};
using TextureOps = EnumSet<TextureOp, TextureOp::Interpolate>;

enum class TextureStageSrc : u8 {
  Current,
  Diffuse,
  Specular,
  SampledTexture,
  TextureFactor,
  Temporary,

  // DeviceCaps.perTextureStageConstant
  StageConstant,
};
constexpr u8 TEXTURE_STAGE_SRC_COUNT {7};

enum class TextureStageSrcMod : u8 {
  None,
  Complement,
  AlphaReplicate,
};
constexpr u8 TEXTURE_STAGE_SRC_MOD_COUNT {3};

enum class TextureStageDestination : u8 {
  Current,
  Temporary,
};
constexpr u8 TEXTURE_STAGE_DESTINATION_COUNT {2};

enum class TextureCoordinateTransformMode : u8 {
  Disabled,
  Count1,
  Count2,
  Count3,
  Count4,
};
constexpr u8 TEXTURE_COORDINATE_TRANSFORM_MODE_COUNT {5};

enum class TransformState : u8 {
  ViewToClip,
  WorldToView,
  LocalToWorld,
  Texture0,
  Texture1,
  Texture2,
  Texture3,
  Texture4,
  Texture5,
  Texture6,
  Texture7,
};
constexpr u8 TRANSFORM_STATE_COUNT {11};

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
constexpr u8 VERTEX_ELEMENT_COUNT {10u};
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
  bool sampler3DMinFilterAnisotropic {false};
  bool sampler3DMagFilterAnisotropic {false};
  u8 samplerMaxAnisotropy {1};
  bool perTextureStageConstant {false};
  TextureOps supportedColorOps;
  TextureOps supportedAlphaOps;
};

struct TextureStageArgument {
  TextureStageSrc src {TextureStageSrc::Current};
  TextureStageSrcMod modifier {TextureStageSrcMod::None};
};

struct TextureCoordinateSet {
  u8 setIndex {};
  TextureCoordinateSrc src {TextureCoordinateSrc::Vertex};
  u8 srcIndex {};
  TextureCoordinateTransformMode transformMode {
    TextureCoordinateTransformMode::Disabled};
  bool projected {false};
};

enum class MaterialColorSource : u8 {
  DiffuseVertexColor,
  SpecularVertexColor,
  Material,
};
constexpr u8 MATERIAL_COLOR_SOURCE_COUNT {3};

// or enum set with wanted features
struct FixedVertexShaderCreateInfo {
  gsl::span<TextureCoordinateSet> textureCoordinateSets {};
  ShadeMode shadeMode {ShadeMode::Gouraud};
  bool lightingEnabled {false};
  bool specularEnabled {false};
  bool vertexColorEnabled {true};
  bool normalizeViewSpaceNormals {false};
  MaterialColorSource diffuseSource {MaterialColorSource::DiffuseVertexColor};
  MaterialColorSource specularSource {MaterialColorSource::SpecularVertexColor};
  MaterialColorSource ambientSource {MaterialColorSource::Material};
  MaterialColorSource emissiveSource {MaterialColorSource::Material};
  FogMode fog {FogMode::None};
  bool fogRangeBased {false};
};

struct TextureStage {
  TextureOp colorOp {TextureOp::Modulate};
  TextureStageArgument colorArg1 {TextureStageSrc::SampledTexture};
  TextureStageArgument colorArg2;
  TextureStageArgument colorArg3;
  TextureOp alphaOp {TextureOp::Replace};
  TextureStageArgument alphaArg1 {TextureStageSrc::SampledTexture};
  TextureStageArgument alphaArg2;
  TextureStageArgument alphaArg3;
  TextureStageDestination dest {TextureStageDestination::Current};
};

struct FixedFragmentShaderCreateInfo {
  gsl::span<const TextureStage> textureStages {};
  // this overrides vertex fog
  FogMode fog {FogMode::None};
};

struct PipelineDescriptor final {
  // null -> default fixed vertex shader
  const FixedVertexShaderCreateInfo* vertexShader {};
  // null -> default fixed fragment shader
  const FixedFragmentShaderCreateInfo* fragmentShader {};
  VertexLayout vertexLayout {};
  PrimitiveType primitiveType {PrimitiveType::PointList};
  CullMode cullMode {CullMode::None};
  FillMode fillMode {FillMode::Solid};
  TestPassCond depthTest {TestPassCond::Always};
  bool depthWriteEnable {false};
  bool dithering {false};
  TestPassCond alphaTest {TestPassCond::Always};
  BlendFactor srcBlendFactor {BlendFactor::One};
  BlendFactor destBlendFactor {BlendFactor::Zero};
  BlendOp blendOp {BlendOp::Add};
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
  Color customBorderColor {};
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

struct DirectionalLightData final {
  Color diffuse;
  Color specular;
  Color ambient;
  Vector3f32 directionInWorld;
};

struct PointLightData final {
  Color diffuse;
  Color specular;
  Color ambient;
  Vector3f32 positionInWorld;
  f32 rangeInWorld {};
  f32 attenuation0 {};
  f32 attenuation1 {};
  f32 attenuation2 {};
};

struct SpotLightData final {
  Color diffuse;
  Color specular;
  Color ambient;
  Vector3f32 positionInWorld;
  Vector3f32 directionInWorld;
  f32 rangeInWorld {};
  f32 attenuation0 {};
  f32 attenuation1 {};
  f32 attenuation2 {};
  f32 falloff {};
  Angle phi;
  Angle theta;
};

using LightData =
  std::variant<PointLightData, SpotLightData, DirectionalLightData>;

} // namespace basalt::gfx
