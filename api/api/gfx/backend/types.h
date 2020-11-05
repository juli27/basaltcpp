#pragma once

#include "api/shared/asserts.h"
#include "api/shared/handle.h"

#include "api/base/types.h"

#include <memory>
#include <vector>

namespace basalt::gfx {

struct CommandList;
struct Composite;

struct Context;
using ContextPtr = std::shared_ptr<Context>;

struct Device;
using DevicePtr = std::shared_ptr<Device>;

enum class CommandType : u8 {
  Clear,
  Draw,
  SetRenderState,
  SetTexture,
  SetTextureStageState,

  // fixed function only
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
    BASALT_ASSERT_MSG(type == T::TYPE, "invalid command cast");
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

enum class RenderState : u8 {
  // vertex state
  CullMode = 0,
  // - fixed function only
  Ambient = 1,
  Lighting = 2,
};
constexpr uSize RENDER_STATE_COUNT = 3u;

enum CullMode : u8 { CullModeNone, CullModeCw, CullModeCcw };

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
  ColorDiffuse1U32,
  ColorSpecular1U32,
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

namespace detail {

struct VertexBufferTag;
struct TextureTag;

} // namespace detail

using VertexBuffer = Handle<detail::VertexBufferTag>;
using Texture = Handle<detail::TextureTag>;

} // namespace basalt::gfx
