#pragma once

#include <runtime/shared/asserts.h>
#include <runtime/shared/handle.h>
#include <runtime/shared/types.h>

#include <vector>

namespace basalt::gfx {

enum class CommandType : u8 {
  Legacy
, SetDirectionalLights
, SetAmbientLight
, SetTransform
, SetRenderState

, ExtRenderDearImGui

, FirstReservedForUserExt = 128
, LastReservedForUserExt = 255
};

struct Command {
  CommandType type;

  template <typename T>
  auto as() -> T& {
    BASALT_ASSERT_MSG(type == T::TYPE, "invalid command cast");
    return *static_cast<T*>(this);
  }

protected:
  constexpr explicit Command(const CommandType t) noexcept
    : type {t} {
  }
};

template <CommandType Type>
struct CommandT : Command {
  static constexpr CommandType TYPE {Type};

  constexpr CommandT() noexcept
    : Command {TYPE} {
  }
};

enum class RenderState : u8 {
  Lighting
, CullMode
};

constexpr uSize RENDER_STATE_COUNT = 2u;

enum CullMode : u8 {
  CullModeNone
, CullModeCw
, CullModeCcw
};

enum RenderFlags : u8 {
  RenderFlagNone = 0x0,
  RenderFlagCullNone = 0x1,
  RenderFlagDisableLighting = 0x2
};

enum class TexCoordinateSrc : u8 {
  Vertex, PositionCameraSpace
};

enum class TransformType : u8 {
  Projection
, View
};

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

struct MeshTypeTag;
struct TextureTypeTag;
struct ModelTypeTag;

} // namespace detail

using MeshHandle = Handle<detail::MeshTypeTag>;
using TextureHandle = Handle<detail::TextureTypeTag>;
using ModelHandle = Handle<detail::ModelTypeTag>;

} // namespace basalt::gfx
