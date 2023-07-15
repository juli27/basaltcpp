#pragma once

#include <basalt/api/gfx/backend/types.h>
#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/api/math/matrix4x4.h>

#include <basalt/api/shared/asserts.h>
#include <basalt/api/shared/color.h>

#include <basalt/api/base/types.h>

#include <gsl/span>

namespace basalt::gfx {

enum class CommandType : u8 {
  ClearAttachments,
  Draw,
  DrawIndexed,
  BindPipeline,
  BindVertexBuffer,
  BindIndexBuffer,
  BindSampler,
  BindTexture,

  // fixed function only
  SetTransform,
  SetAmbientLight,
  SetLights,
  SetMaterial,
  SetFogParameters,

  // built-in extensions
  ExtDrawXMesh,
  ExtRenderDearImGui,
};

struct Command {
  const CommandType type;

  template <typename T>
  [[nodiscard]] auto as() const -> const T& {
    BASALT_ASSERT(type == T::TYPE, "invalid command cast");
    return *static_cast<const T*>(this);
  }

protected:
  constexpr explicit Command(const CommandType t) noexcept : type {t} {
  }
};

template <CommandType Type>
struct CommandT : Command {
  static constexpr CommandType TYPE {Type};

  constexpr CommandT() noexcept : Command {TYPE} {
  }
};

struct CommandClearAttachments final : CommandT<CommandType::ClearAttachments> {
  Attachments attachments;
  Color color;
  f32 depth;
  u32 stencil;

  constexpr CommandClearAttachments(const Attachments aAttachments,
                                    const Color& aColor, const f32 aDepth,
                                    const u32 aStencil) noexcept
    : attachments {aAttachments}
    , color {aColor}
    , depth {aDepth}
    , stencil {aStencil} {
  }
};

struct CommandDraw final : CommandT<CommandType::Draw> {
  u32 firstVertex;
  u32 vertexCount;

  constexpr CommandDraw(const u32 aFirstVertex, const u32 aVertexCount) noexcept
    : firstVertex {aFirstVertex}, vertexCount {aVertexCount} {
  }
};

struct CommandDrawIndexed final : CommandT<CommandType::DrawIndexed> {
  i32 vertexOffset;
  u32 minIndex;
  u32 numVertices;
  u32 firstIndex;
  u32 indexCount;

  constexpr CommandDrawIndexed(const i32 aVertexOffset, const u32 aMinIndex,
                               const u32 aNumVertices, const u32 aFirstIndex,
                               const u32 aIndexCount) noexcept
    : vertexOffset {aVertexOffset}
    , minIndex {aMinIndex}
    , numVertices {aNumVertices}
    , firstIndex {aFirstIndex}
    , indexCount {aIndexCount} {
  }
};

struct CommandBindPipeline final : CommandT<CommandType::BindPipeline> {
  Pipeline pipelineId;

  constexpr explicit CommandBindPipeline(const Pipeline aPipelineId) noexcept
    : pipelineId {aPipelineId} {
  }
};

struct CommandBindVertexBuffer final : CommandT<CommandType::BindVertexBuffer> {
  VertexBuffer vertexBufferId;
  uDeviceSize offsetInBytes;

  constexpr CommandBindVertexBuffer(const VertexBuffer aVertexBufferId,
                                    const uDeviceSize aOffsetInBytes) noexcept
    : vertexBufferId {aVertexBufferId}, offsetInBytes {aOffsetInBytes} {
  }
};

struct CommandBindIndexBuffer final : CommandT<CommandType::BindIndexBuffer> {
  IndexBuffer indexBufferId;

  constexpr explicit CommandBindIndexBuffer(
    const IndexBuffer aIndexBufferId) noexcept
    : indexBufferId {aIndexBufferId} {
  }
};

struct CommandBindSampler final : CommandT<CommandType::BindSampler> {
  Sampler samplerId;

  constexpr explicit CommandBindSampler(const Sampler aSamplerId) noexcept
    : samplerId {aSamplerId} {
  }
};

struct CommandBindTexture final : CommandT<CommandType::BindTexture> {
  Texture textureId;

  constexpr explicit CommandBindTexture(const Texture aTextureId) noexcept
    : textureId {aTextureId} {
  }
};

struct CommandSetTransform final : CommandT<CommandType::SetTransform> {
  TransformState transformState;
  Matrix4x4f32 transform;

  constexpr CommandSetTransform(const TransformState aTransformState,
                                const Matrix4x4f32& aTransform) noexcept
    : transformState {aTransformState}, transform {aTransform} {
  }
};

struct CommandSetAmbientLight final : CommandT<CommandType::SetAmbientLight> {
  Color ambient;

  constexpr explicit CommandSetAmbientLight(const Color& aAmbient) noexcept
    : ambient {aAmbient} {
  }
};

struct CommandSetLights final : CommandT<CommandType::SetLights> {
  gsl::span<const Light> lights;

  constexpr explicit CommandSetLights(
    const gsl::span<const Light> aLights) noexcept
    : lights {aLights} {
  }
};

struct CommandSetMaterial final : CommandT<CommandType::SetMaterial> {
  Color diffuse;
  Color ambient;
  Color emissive;
  Color specular;
  f32 specularPower;

  constexpr CommandSetMaterial(const Color& aDiffuse, const Color& aAmbient,
                               const Color& aEmissive, const Color& aSpecular,
                               const f32 aSpecularPower) noexcept
    : diffuse {aDiffuse}
    , ambient {aAmbient}
    , emissive {aEmissive}
    , specular {aSpecular}
    , specularPower {aSpecularPower} {
  }
};

struct CommandSetFogParameters final : CommandT<CommandType::SetFogParameters> {
  Color color;
  f32 start;
  f32 end;
  f32 density;

  constexpr CommandSetFogParameters(const Color& aColor, const f32 aStart,
                                    const f32 aEnd, const f32 aDensity) noexcept
    : color {aColor}, start {aStart}, end {aEnd}, density {aDensity} {
  }
};

#define VISIT(cmdStruct)                                                       \
  case cmdStruct::TYPE:                                                        \
    visitor(cmd.as<cmdStruct>());                                              \
    return

template <typename Visitor>
auto visit(const Command& cmd, Visitor&& visitor) -> void {
  switch (cmd.type) {
    VISIT(CommandClearAttachments);
    VISIT(CommandDraw);
    VISIT(CommandDrawIndexed);
    VISIT(CommandBindPipeline);
    VISIT(CommandBindVertexBuffer);
    VISIT(CommandBindIndexBuffer);
    VISIT(CommandBindSampler);
    VISIT(CommandBindTexture);
    VISIT(CommandSetTransform);
    VISIT(CommandSetAmbientLight);
    VISIT(CommandSetLights);
    VISIT(CommandSetMaterial);
    VISIT(CommandSetFogParameters);

  default:
    break;
  }

  visitor(cmd);
}

#undef VISIT

namespace ext {

struct CommandDrawXMesh final : CommandT<CommandType::ExtDrawXMesh> {
  XMesh xMeshId;
  u32 subset;

  constexpr CommandDrawXMesh(const XMesh aXMeshId, const u32 aSubset) noexcept
    : xMeshId {aXMeshId}, subset {aSubset} {
  }
};

struct CommandRenderDearImGui final
  : CommandT<CommandType::ExtRenderDearImGui> {};

} // namespace ext

} // namespace basalt::gfx
