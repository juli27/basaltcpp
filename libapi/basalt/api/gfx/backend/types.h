#pragma once

#include <basalt/api/shared/color.h>
#include <basalt/api/shared/handle.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/vector3.h>

#include <basalt/api/base/enum_set.h>
#include <basalt/api/base/types.h>

#include <gsl/span>

#include <array>
#include <memory>
#include <variant>
#include <vector>

namespace basalt::gfx {

struct AdapterExclusiveModeInfo;
struct AdapterIdentifier;
struct AdapterInfo;
struct AdapterSharedModeInfo;
struct BackBufferFormat;
struct DisplayMode;
struct PciId;

struct IndexBufferCreateInfo;
BASALT_DEFINE_HANDLE(IndexBufferHandle);

struct PipelineCreateInfo;
BASALT_DEFINE_HANDLE(PipelineHandle);

struct VertexBufferCreateInfo;
BASALT_DEFINE_HANDLE(VertexBufferHandle);

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
auto inline constexpr BLEND_FACTOR_COUNT = u8{8};

enum class BlendOp : u8 {
  Add,
  Subtract,
  ReverseSubtract,
};
auto inline constexpr BLEND_OP_COUNT = u8{3};

enum class CullMode : u8 {
  None,
  Clockwise,
  CounterClockwise,
};
auto inline constexpr CULL_MODE_COUNT = u8{3};

enum class FillMode : u8 {
  Point,
  Wireframe,
  Solid,
};
auto inline constexpr FILL_MODE_COUNT = u8{3};

struct FixedFragmentShaderCreateInfo;
struct FixedVertexShaderCreateInfo;

enum class IndexType : u8 {
  U16,
  U32,
};
auto inline constexpr INDEX_TYPE_COUNT = u8{2};
using IndexTypes = EnumSet<IndexType, IndexType::U32>;

enum class PrimitiveType : u8 {
  PointList,
  LineList,
  LineStrip,
  TriangleList,
  TriangleStrip,
  TriangleFan,
};
auto inline constexpr PRIMITIVE_TYPE_COUNT = u8{6};

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
auto inline constexpr STENCIL_OP_COUNT = u8{8};

struct StencilOpState;

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
auto inline constexpr TEST_PASS_COND_COUNT = u8{8};

enum class FogMode : u8 {
  None,
  Linear,
  Exponential,
  ExponentialSquared,
};
auto inline constexpr FOG_MODE_COUNT = u8{4};

enum class MaterialColorSource : u8 {
  DiffuseVertexColor,
  SpecularVertexColor,
  Material,
};
auto inline constexpr MATERIAL_COLOR_SOURCE_COUNT = u8{3};

enum class ShadeMode : u8 {
  Flat,
  Gouraud,
};
auto inline constexpr SHADE_MODE_COUNT = u8{2};

struct TextureCoordinateSet;

enum class TextureCoordinateSrc : u8 {
  Vertex,
  PositionInViewSpace,
  NormalInViewSpace,
  ReflectionVectorInViewSpace,
};
auto inline constexpr TEXTURE_COORDINATE_SOURCE_COUNT = u8{4};

enum class TextureCoordinateTransformMode : u8 {
  Disabled,
  Count1,
  Count2,
  Count3,
  Count4,
};
auto inline constexpr TEXTURE_COORDINATE_TRANSFORM_MODE_COUNT = u8{5};

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
auto inline constexpr TEXTURE_OP_COUNT = u8{24};
using TextureOps = EnumSet<TextureOp, TextureOp::Interpolate>;

struct TextureStage;
struct TextureStageArgument;

enum class TextureStageDestination : u8 {
  Current,
  Temporary,
};
auto inline constexpr TEXTURE_STAGE_DESTINATION_COUNT = u8{2};

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
auto inline constexpr TEXTURE_STAGE_SRC_COUNT = u8{7};

enum class TextureStageSrcMod : u8 {
  None,
  Complement,
  AlphaReplicate,
};
auto inline constexpr TEXTURE_STAGE_SRC_MOD_COUNT = u8{3};

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
constexpr auto BORDER_COLOR_COUNT = u8{4};

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

BASALT_DEFINE_HANDLE(SamplerHandle);
BASALT_DEFINE_HANDLE(TextureHandle);

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

// listed from left to right, in c-array order if power of 2 aligned,
// lsb to msb otherwise
enum class ImageFormat : u8 {
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
  D15S1,
  D16,

  // 32-bit
  D24X8,
  D24X4S4,
  D24S8,
  D24FS8,
  D32,
};
constexpr auto IMAGE_FORMAT_COUNT = u8{14};

enum class MultiSampleCount : u8 {
  One,
  Two,
  Three,
  Four,
  Five,
  Six,
  Seven,
  Eight,
  Nine,
  Ten,
  Eleven,
  Twelve,
  Thirteen,
  Fourteen,
  Fifteen,
  Sixteen,
};
constexpr auto MULTI_SAMPLE_COUNT_COUNT = u8{16};

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
