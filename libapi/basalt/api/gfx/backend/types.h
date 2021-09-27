#pragma once

#include <basalt/api/shared/asserts.h>
#include <basalt/api/shared/color.h>
#include <basalt/api/shared/handle.h>

#include <basalt/api/base/enum_set.h>
#include <basalt/api/base/types.h>

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
  BindTexture,
  BindSampler,

  // fixed function only
  SetTextureStageState,
  SetDirectionalLights,
  SetTransform,
  SetMaterial,

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
  // vertex state
  CullMode,
  // - fixed function only
  Ambient,
  Lighting,

  // pixel state
  FillMode,
  DepthTest,
  DepthWrite,
  // - fixed function only
  ShadeMode,
};
constexpr uSize RENDER_STATE_COUNT = 7u;

enum class ShadeMode : u8 {
  Flat,
  Gouraud,
};
constexpr uSize SHADE_MODE_COUNT = 2u;

enum TexCoordinateSrc : u8 {
  TcsVertex = 0,
  TcsVertexPositionCameraSpace = 1,
};

// TODO: support border and custom border color (no/partial support in Vulkan
// and OpenGL ES)
// TODO: support MirrorOnce
enum class TextureAddressMode {
  WrapRepeat,
  MirrorRepeat,
  ClampEdge,
};
constexpr uSize TEXTURE_ADDRESS_MODE_COUNT {3u};

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

enum class TextureStageState : u8 {
  CoordinateSource = 0,
  TextureTransformFlags = 1,
};
constexpr uSize TEXTURE_STAGE_STATE_COUNT = 2u;

enum TextureTransformFlags : u8 {
  TtfDisabled = 0,
  TtfCount4 = 1,

  TtfProjected = 0x2,
};

enum class TransformState : u8 {
  Projection,
  View,
  World,
  Texture,
};
constexpr uSize TRANSFORM_STATE_COUNT = 4u;

enum class VertexElement : u8 {
  Position3F32,
  PositionTransformed4F32,
  Normal3F32,
  ColorDiffuseA8R8G8B8_U32,
  ColorSpecularA8R8G8B8_U32,
  TextureCoords2F32
};
using VertexLayout = std::vector<VertexElement>;

namespace detail {
struct SamplerTag;
struct TextureTag;
struct VertexBufferTag;
} // namespace detail
using Sampler = Handle<detail::SamplerTag>;
using Texture = Handle<detail::TextureTag>;
using VertexBuffer = Handle<detail::VertexBufferTag>;

struct CommandList;
using Composite = std::vector<CommandList>;

struct Context;
using ContextPtr = std::unique_ptr<Context>;

struct Device;

struct RenderState;
using RenderStateValue =
  std::variant<bool, CullMode, FillMode, ShadeMode, DepthTestPass, Color>;

struct Command {
  const CommandType type;

  template <typename T>
  auto as() -> T& {
    BASALT_ASSERT(type == T::TYPE, "invalid command cast");
    return *static_cast<T*>(this);
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

struct SamplerDescriptor final {
  TextureFilter filter {TextureFilter::Point};
  TextureMipFilter mipFilter {TextureMipFilter::None};
  TextureAddressMode addressModeU {TextureAddressMode::WrapRepeat};
  TextureAddressMode addressModeV {TextureAddressMode::WrapRepeat};
  TextureAddressMode addressModeW {TextureAddressMode::WrapRepeat};
};

} // namespace basalt::gfx
