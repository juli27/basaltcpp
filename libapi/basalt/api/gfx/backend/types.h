#pragma once

#include "basalt/api/shared/color.h"
#include "basalt/api/shared/handle.h"

#include "basalt/api/math/angle.h"
#include "basalt/api/math/vector3.h"

#include "basalt/api/base/enum_set.h"
#include "basalt/api/base/types.h"

#include <gsl/span>

#include <array>
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
constexpr auto BLEND_FACTOR_COUNT = u8{8};

enum class BlendOp : u8 {
  Add,
  Subtract,
  ReverseSubtract,
};
constexpr auto BLEND_OP_COUNT = u8{3};

enum class BorderColor : u8 {
  BlackTransparent,
  BlackOpaque,
  WhiteOpaque,

  // DeviceCaps.samplerCustomBorderColor
  Custom,
};
constexpr auto BORDER_COLOR_COUNT = u8{4};

enum class CullMode : u8 {
  None,
  Clockwise,
  CounterClockwise,
};
constexpr auto CULL_MODE_COUNT = u8{3};

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
constexpr auto TEST_PASS_COND_COUNT = u8{8};

enum class FillMode : u8 {
  Point,
  Wireframe,
  Solid,
};
constexpr auto FILL_MODE_COUNT = u8{3};

enum class PrimitiveType : u8 {
  PointList,
  LineList,
  LineStrip,
  TriangleList,
  TriangleStrip,
  TriangleFan,
};
constexpr auto PRIMITIVE_TYPE_COUNT = u8{6};

enum class ShadeMode : u8 {
  Flat,
  Gouraud,
};
constexpr auto SHADE_MODE_COUNT = u8{2};

enum class FogMode : u8 {
  None,
  Linear,
  Exponential,
  ExponentialSquared,
};
constexpr auto FOG_MODE_COUNT = u8{4};

enum class TextureAddressMode : u8 {
  Repeat,
  Mirror,
  ClampToEdge,

  // DeviceCaps.samplerClampToBorder
  ClampToBorder,
  // DeviceCaps.samplerMirrorOnceClampToEdge
  MirrorOnceClampToEdge,
};
constexpr auto TEXTURE_ADDRESS_MODE_COUNT = u8{5};

enum class TextureCoordinateSrc : u8 {
  Vertex,
  PositionInViewSpace,
  NormalInViewSpace,
  ReflectionVectorInViewSpace,
};
constexpr auto TEXTURE_COORDINATE_SOURCE_COUNT = u8{4};

enum class TextureFilter : u8 {
  Point,
  Bilinear,

  // DeviceCaps.samplerMinFilterAnisotropic
  // DeviceCaps.samplerMagFilterAnisotropic
  Anisotropic,
};
constexpr auto TEXTURE_FILTER_COUNT = u8{3};

enum class TextureMipFilter : u8 {
  None,
  Point,
  Linear,
};
constexpr auto TEXTURE_MIP_FILTER_COUNT = u8{3};

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
  // performs per-pixel bump mapping using the env map in the next stage with
  // luminance = 1
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
constexpr auto TEXTURE_OP_COUNT = u8{24};
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
constexpr auto TEXTURE_STAGE_SRC_COUNT = u8{7};

enum class TextureStageSrcMod : u8 {
  None,
  Complement,
  AlphaReplicate,
};
constexpr auto TEXTURE_STAGE_SRC_MOD_COUNT = u8{3};

enum class TextureStageDestination : u8 {
  Current,
  Temporary,
};
constexpr auto TEXTURE_STAGE_DESTINATION_COUNT = u8{2};

enum class TextureCoordinateTransformMode : u8 {
  Disabled,
  Count1,
  Count2,
  Count3,
  Count4,
};
constexpr auto TEXTURE_COORDINATE_TRANSFORM_MODE_COUNT = u8{5};

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
constexpr auto TRANSFORM_STATE_COUNT = u8{11};

enum class VertexElement : u8;

template <typename Container>
class VertexLayout;

template <uSize Num>
using VertexLayoutArray = VertexLayout<std::array<VertexElement, Num>>;

using VertexLayoutVector = VertexLayout<std::vector<VertexElement>>;

// TODO: move this to vertex_layout.h
class VertexLayoutSpan {
  using SpanType = gsl::span<VertexElement const>;

public:
  using iterator = typename SpanType::iterator;

  constexpr VertexLayoutSpan() = default;

  template <typename Container>
  /* implicit */ constexpr VertexLayoutSpan(
    VertexLayout<Container> const& layout)
    : mAttributes{layout.attributes()} {
  }

  /* implicit */ constexpr operator gsl::span<VertexElement const>() {
    return mAttributes;
  }

  constexpr auto empty() const -> bool {
    return mAttributes.empty();
  }

  constexpr auto begin() const -> iterator {
    return mAttributes.begin();
  }

  constexpr auto end() const -> iterator {
    return mAttributes.end();
  }

private:
  gsl::span<VertexElement const> mAttributes;
};

enum class IndexType : u8 {
  U16,
  U32,
};
constexpr auto INDEX_TYPE_COUNT = u8{2};

using IndexTypes = EnumSet<IndexType, IndexType::U32>;

BASALT_DEFINE_HANDLE(PipelineHandle);
BASALT_DEFINE_HANDLE(SamplerHandle);
BASALT_DEFINE_HANDLE(TextureHandle);
BASALT_DEFINE_HANDLE(VertexBufferHandle);
BASALT_DEFINE_HANDLE(IndexBufferHandle);

struct Command;
class CommandList;

class Device;
using DevicePtr = std::shared_ptr<Device>;
class SwapChain;
using SwapChainPtr = std::shared_ptr<SwapChain>;

using uDeviceSize = u64;

struct DeviceCaps final {
  uDeviceSize maxVertexBufferSizeInBytes{};
  uDeviceSize maxIndexBufferSizeInBytes{};
  IndexTypes supportedIndexTypes{IndexType::U16};
  u32 maxLights{1};
  u32 maxTextureBlendStages{1};
  u32 maxBoundSampledTextures{1};
  bool samplerClampToBorder{false};
  bool samplerCustomBorderColor{false};
  bool samplerMirrorOnceClampToEdge{false};
  bool samplerMinFilterAnisotropic{false};
  bool samplerMagFilterAnisotropic{false};
  bool samplerCubeMinFilterAnisotropic{false};
  bool samplerCubeMagFilterAnisotropic{false};
  bool sampler3DMinFilterAnisotropic{false};
  bool sampler3DMagFilterAnisotropic{false};
  u8 samplerMaxAnisotropy{1};
  bool perTextureStageConstant{false};
  TextureOps supportedColorOps;
  TextureOps supportedAlphaOps;
};

struct TextureStageArgument {
  TextureStageSrc src{TextureStageSrc::Current};
  TextureStageSrcMod modifier{TextureStageSrcMod::None};
};

struct TextureCoordinateSet {
  u8 stageIndex{};
  TextureCoordinateSrc src{TextureCoordinateSrc::Vertex};
  u8 srcIndex{};
  TextureCoordinateTransformMode transformMode{
    TextureCoordinateTransformMode::Disabled};
  bool projected{false};
};

enum class MaterialColorSource : u8 {
  DiffuseVertexColor,
  SpecularVertexColor,
  Material,
};
constexpr auto MATERIAL_COLOR_SOURCE_COUNT = u8{3};

// or enum set with wanted features
struct FixedVertexShaderCreateInfo {
  gsl::span<TextureCoordinateSet const> textureCoordinateSets{};
  ShadeMode shadeMode{ShadeMode::Gouraud};
  bool lightingEnabled{false};
  bool specularEnabled{false};
  bool vertexColorEnabled{true};
  bool normalizeViewSpaceNormals{false};
  MaterialColorSource diffuseSource{MaterialColorSource::DiffuseVertexColor};
  MaterialColorSource specularSource{MaterialColorSource::SpecularVertexColor};
  MaterialColorSource ambientSource{MaterialColorSource::Material};
  MaterialColorSource emissiveSource{MaterialColorSource::Material};
  FogMode fog{FogMode::None};
  bool fogRangeBased{false};
};

struct TextureStage {
  TextureOp colorOp{TextureOp::Modulate};
  TextureStageArgument colorArg1{TextureStageSrc::SampledTexture};
  TextureStageArgument colorArg2{};
  TextureStageArgument colorArg3{};
  TextureOp alphaOp{TextureOp::Replace};
  TextureStageArgument alphaArg1{TextureStageSrc::SampledTexture};
  TextureStageArgument alphaArg2{};
  TextureStageArgument alphaArg3{};
  TextureStageDestination dest{TextureStageDestination::Current};
  // TODO: replace with Matrix2x2
  f32 bumpEnvMat00{1};
  f32 bumpEnvMat01{0};
  f32 bumpEnvMat10{0};
  f32 bumpEnvMat11{1};
  f32 bumpEnvLuminanceScale{1};
  f32 bumpEnvLuminanceOffset{0};
};

struct FixedFragmentShaderCreateInfo {
  gsl::span<TextureStage const> textureStages{};
  // this overrides vertex fog
  FogMode fog{FogMode::None};
};

enum class StencilOp : u8 {
  Keep,
  Zero,
  Replace,
  Invert,
  IncrementClamp,
  DecrementClamp,
  IncrementWrap,
  DecrementWrap,
};
constexpr auto STENCIL_OP_COUNT = u8{8};

struct StencilOpState {
  TestPassCond test{TestPassCond::Always};
  StencilOp failOp{StencilOp::Keep};
  StencilOp passDepthFailOp{StencilOp::Keep};
  StencilOp passDepthPassOp{StencilOp::Keep};
};

struct PipelineCreateInfo final {
  // null -> default fixed vertex shader
  FixedVertexShaderCreateInfo const* vertexShader{};
  // null -> default fixed fragment shader
  FixedFragmentShaderCreateInfo const* fragmentShader{};
  VertexLayoutSpan vertexLayout;
  PrimitiveType primitiveType{PrimitiveType::PointList};
  CullMode cullMode{CullMode::None};
  FillMode fillMode{FillMode::Solid};
  TestPassCond depthTest{TestPassCond::Always};
  bool depthWriteEnable{false};
  StencilOpState frontFaceStencilOp{};
  StencilOpState backFaceStencilOp{};
  bool dithering{false};
  TestPassCond alphaTest{TestPassCond::Always};
  BlendFactor srcBlendFactor{BlendFactor::One};
  BlendFactor destBlendFactor{BlendFactor::Zero};
  BlendOp blendOp{BlendOp::Add};
};

struct SamplerCreateInfo final {
  TextureFilter magFilter{TextureFilter::Point};
  TextureFilter minFilter{TextureFilter::Point};
  TextureMipFilter mipFilter{TextureMipFilter::None};
  TextureAddressMode addressModeU{TextureAddressMode::Repeat};
  TextureAddressMode addressModeV{TextureAddressMode::Repeat};
  TextureAddressMode addressModeW{TextureAddressMode::Repeat};

  // DeviceCaps.samplerClampToBorder
  BorderColor borderColor{BorderColor::BlackTransparent};
  // DeviceCaps.samplerCustomBorderColor
  Color customBorderColor{};
  // DeviceCaps.samplerMaxAnisotropy
  u8 maxAnisotropy{1};
};

struct VertexBufferCreateInfo final {
  uDeviceSize sizeInBytes{};
  VertexLayoutSpan layout;
};

struct IndexBufferCreateInfo final {
  uDeviceSize sizeInBytes{};
  IndexType type{IndexType::U16};
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
  f32 rangeInWorld{};
  f32 attenuation0{};
  f32 attenuation1{};
  f32 attenuation2{};
};

struct SpotLightData final {
  Color diffuse;
  Color specular;
  Color ambient;
  Vector3f32 positionInWorld;
  Vector3f32 directionInWorld;
  f32 rangeInWorld{};
  f32 attenuation0{};
  f32 attenuation1{};
  f32 attenuation2{};
  f32 falloff{};
  Angle phi;
  Angle theta;
};

using LightData =
  std::variant<PointLightData, SpotLightData, DirectionalLightData>;

} // namespace basalt::gfx
