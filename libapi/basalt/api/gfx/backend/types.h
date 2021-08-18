#pragma once

#include <basalt/api/shared/asserts.h>
#include <basalt/api/shared/color.h>
#include <basalt/api/shared/handle.h>

#include <basalt/api/base/types.h>

#include <memory>
#include <variant>
#include <vector>

namespace basalt::gfx {

struct CommandList;

using Composite = std::vector<CommandList>;

struct Context;
using ContextPtr = std::shared_ptr<Context>;

struct Device;
using DevicePtr = std::shared_ptr<Device>;

enum class ContextStatus : u8 {
  Ok,
  Error,
  DeviceLost,
  ResetNeeded,
};

enum class PresentResult : u8 {
  Ok,
  DeviceLost,
};

enum class CommandType : u8 {
  Clear,
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

enum class CullMode : u8 {
  None,
  Clockwise,
  CounterClockwise,
};
constexpr uSize CULL_MODE_COUNT = 3u;

enum class FillMode : u8 {
  Point,
  Wireframe,
  Solid,
};
constexpr uSize FILL_MODE_COUNT = 3u;

enum class ShadeMode : u8 {
  Flat,
  Gouraud,
};
constexpr uSize SHADE_MODE_COUNT = 2u;

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

using RenderStateValue =
  std::variant<bool, CullMode, FillMode, ShadeMode, DepthTestPass, Color>;

struct RenderState;

enum class TextureStageState : u8 {
  CoordinateSource = 0,
  TextureTransformFlags = 1,
};
constexpr uSize TEXTURE_STAGE_STATE_COUNT = 2u;

enum TexCoordinateSrc : u8 { TcsVertex = 0, TcsVertexPositionCameraSpace = 1 };
enum TextureTransformFlags : u8 {
  TtfDisabled = 0,
  TtfCount4 = 1,

  TtfProjected = 0x2,
};

enum class TransformState : u8 { Projection, View, World, Texture };
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

enum class PrimitiveType : u8 {
  PointList,
  LineList,
  LineStrip,
  TriangleList,
  TriangleStrip,
  TriangleFan,
};
constexpr uSize PRIMITIVE_TYPE_COUNT = 6u;

enum class TextureFilter : u8 {
  Point,
  Linear,
  Anisotropic, // TODO: check for support and set level
};
constexpr uSize TEXTURE_FILTER_COUNT {3u};

enum class TextureMipFilter : u8 {
  None,
  Point,
  Linear,
};
constexpr uSize TEXTURE_MIP_FILTER_COUNT {3u};

struct SamplerDescription final {
  TextureFilter minFilter {TextureFilter::Point};
  TextureFilter magFilter {TextureFilter::Point};
  TextureMipFilter mipFilter {TextureMipFilter::None};
};

namespace detail {

struct VertexBufferTag;
struct TextureTag;
struct SamplerTag;

} // namespace detail

using VertexBuffer = Handle<detail::VertexBufferTag>;
using Texture = Handle<detail::TextureTag>;
using Sampler = Handle<detail::SamplerTag>;

} // namespace basalt::gfx
