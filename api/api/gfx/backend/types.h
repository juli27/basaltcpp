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
  Legacy,
  SetDirectionalLights,
  SetTransform,
  SetRenderState,

  ExtDrawXModel,
  ExtRenderDearImGui,

  FirstReservedForUserExt = 128,
  LastReservedForUserExt = 255,
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

enum class RenderState : u8 { Lighting, Ambient, CullMode };
constexpr uSize RENDER_STATE_COUNT = 3u;

enum CullMode : u8 { CullModeNone, CullModeCw, CullModeCcw };

enum RenderFlags : u8 {
  RenderFlagNone = 0x0,
  RenderFlagCullNone = 0x1,
  RenderFlagDisableLighting = 0x2
};

enum class TexCoordinateSrc : u8 { Vertex, PositionCameraSpace };

enum class TransformType : u8 { Projection, View, World, Texture };

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
  TriangleFan
};

namespace detail {

struct MeshTag;
struct TextureTag;

} // namespace detail

using MeshHandle = Handle<detail::MeshTag>;
using TextureHandle = Handle<detail::TextureTag>;

} // namespace basalt::gfx
