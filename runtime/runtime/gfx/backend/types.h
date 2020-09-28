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

, FirstReservedForExt = 128
, LastReservedForExt = 255
};

struct Command {
  CommandType type;

  Command() = delete;

  constexpr explicit Command(const CommandType t) noexcept
    : type {t} {
  }

  constexpr Command(const Command&) noexcept = default;
  constexpr Command(Command&&) noexcept = default;

  ~Command() noexcept = default;

  auto operator=(const Command&) noexcept -> Command& = default;
  auto operator=(Command&&) noexcept -> Command& = default;

  template <typename T>
  auto as() -> T& {
    BASALT_ASSERT_MSG(type == T::TYPE, "invalid command cast");
    return *static_cast<T*>(this);
  }
};

template <CommandType Type>
struct CommandT : Command {
  static constexpr CommandType TYPE = Type;

  constexpr CommandT() noexcept
    : Command {TYPE} {
  }

  constexpr CommandT(const CommandT&) noexcept = default;
  constexpr CommandT(CommandT&&) noexcept = default;

  ~CommandT() noexcept = default;

  auto operator=(const CommandT&) noexcept -> CommandT& = default;
  auto operator=(CommandT&&) noexcept -> CommandT& = default;
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
