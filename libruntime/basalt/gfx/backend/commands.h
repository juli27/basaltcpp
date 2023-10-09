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
  SetStencilReference,
  SetStencilReadMask,
  SetStencilWriteMask,
  SetBlendConstant,

  // fixed function only
  SetTransform,
  SetAmbientLight,
  SetLights,
  SetMaterial,
  SetFogParameters,
  SetReferenceAlpha,
  SetTextureFactor,
  SetTextureStageConstant,

  // built-in extensions
  ExtDrawXMesh,
  ExtRenderDearImGui,
  ExtBeginEffect,
  ExtEndEffect,
  ExtBeginEffectPass,
  ExtEndEffectPass,
};

struct Command {
  CommandType const type;

  template <typename T>
  [[nodiscard]] auto as() const -> T const& {
    BASALT_ASSERT(type == T::TYPE, "invalid command cast");
    return *static_cast<T const*>(this);
  }

protected:
  constexpr explicit Command(CommandType const t) noexcept : type{t} {
  }
};

template <CommandType Type>
struct CommandT : Command {
  static constexpr auto TYPE = Type;

  constexpr CommandT() noexcept : Command{TYPE} {
  }
};

struct CommandClearAttachments final : CommandT<CommandType::ClearAttachments> {
  Attachments attachments;
  Color color;
  f32 depth;
  u32 stencil;

  constexpr CommandClearAttachments(Attachments const aAttachments,
                                    Color const& aColor, f32 const aDepth,
                                    u32 const aStencil) noexcept
    : attachments{aAttachments}
    , color{aColor}
    , depth{aDepth}
    , stencil{aStencil} {
  }
};

struct CommandDraw final : CommandT<CommandType::Draw> {
  u32 firstVertex;
  u32 vertexCount;

  constexpr CommandDraw(u32 const aFirstVertex, u32 const aVertexCount) noexcept
    : firstVertex{aFirstVertex}
    , vertexCount{aVertexCount} {
  }
};

struct CommandDrawIndexed final : CommandT<CommandType::DrawIndexed> {
  i32 vertexOffset;
  u32 minIndex;
  u32 numVertices;
  u32 firstIndex;
  u32 indexCount;

  constexpr CommandDrawIndexed(i32 const aVertexOffset, u32 const aMinIndex,
                               u32 const aNumVertices, u32 const aFirstIndex,
                               u32 const aIndexCount) noexcept
    : vertexOffset{aVertexOffset}
    , minIndex{aMinIndex}
    , numVertices{aNumVertices}
    , firstIndex{aFirstIndex}
    , indexCount{aIndexCount} {
  }
};

struct CommandBindPipeline final : CommandT<CommandType::BindPipeline> {
  Pipeline pipelineId;

  constexpr explicit CommandBindPipeline(Pipeline const aPipelineId) noexcept
    : pipelineId{aPipelineId} {
  }
};

struct CommandBindVertexBuffer final : CommandT<CommandType::BindVertexBuffer> {
  VertexBuffer vertexBufferId;
  uDeviceSize offsetInBytes;

  constexpr CommandBindVertexBuffer(VertexBuffer const aVertexBufferId,
                                    uDeviceSize const aOffsetInBytes) noexcept
    : vertexBufferId{aVertexBufferId}
    , offsetInBytes{aOffsetInBytes} {
  }
};

struct CommandBindIndexBuffer final : CommandT<CommandType::BindIndexBuffer> {
  IndexBuffer indexBufferId;

  constexpr explicit CommandBindIndexBuffer(
    IndexBuffer const aIndexBufferId) noexcept
    : indexBufferId{aIndexBufferId} {
  }
};

struct CommandBindSampler final : CommandT<CommandType::BindSampler> {
  u8 slot;
  Sampler samplerId;

  constexpr CommandBindSampler(u8 const aSlot,
                               Sampler const aSamplerId) noexcept
    : slot{aSlot}
    , samplerId{aSamplerId} {
  }
};

struct CommandBindTexture final : CommandT<CommandType::BindTexture> {
  u8 slot;
  Texture textureId;

  constexpr CommandBindTexture(u8 const aSlot,
                               Texture const aTextureId) noexcept
    : slot{aSlot}
    , textureId{aTextureId} {
  }
};

struct CommandSetStencilReference final
  : CommandT<CommandType::SetStencilReference> {
  u32 value;

  constexpr explicit CommandSetStencilReference(u32 const aValue) noexcept
    : value{aValue} {
  }
};

struct CommandSetStencilReadMask final
  : CommandT<CommandType::SetStencilReadMask> {
  u32 value;

  constexpr explicit CommandSetStencilReadMask(u32 const aValue) noexcept
    : value{aValue} {
  }
};

struct CommandSetStencilWriteMask final
  : CommandT<CommandType::SetStencilWriteMask> {
  u32 value;

  constexpr explicit CommandSetStencilWriteMask(u32 const aValue) noexcept
    : value{aValue} {
  }
};

struct CommandSetBlendConstant final : CommandT<CommandType::SetBlendConstant> {
  Color value;

  constexpr explicit CommandSetBlendConstant(Color const& aValue) noexcept
    : value{aValue} {
  }
};

struct CommandSetTransform final : CommandT<CommandType::SetTransform> {
  TransformState transformState;
  Matrix4x4f32 transform;

  constexpr CommandSetTransform(TransformState const aTransformState,
                                Matrix4x4f32 const& aTransform) noexcept
    : transformState{aTransformState}
    , transform{aTransform} {
  }
};

struct CommandSetAmbientLight final : CommandT<CommandType::SetAmbientLight> {
  Color ambient;

  constexpr explicit CommandSetAmbientLight(Color const& aAmbient) noexcept
    : ambient{aAmbient} {
  }
};

struct CommandSetLights final : CommandT<CommandType::SetLights> {
  gsl::span<LightData const> lights;

  constexpr explicit CommandSetLights(
    gsl::span<LightData const> const aLights) noexcept
    : lights{aLights} {
  }
};

struct CommandSetMaterial final : CommandT<CommandType::SetMaterial> {
  Color diffuse;
  Color ambient;
  Color emissive;
  Color specular;
  f32 specularPower;

  constexpr CommandSetMaterial(Color const& aDiffuse, Color const& aAmbient,
                               Color const& aEmissive, Color const& aSpecular,
                               f32 const aSpecularPower) noexcept
    : diffuse{aDiffuse}
    , ambient{aAmbient}
    , emissive{aEmissive}
    , specular{aSpecular}
    , specularPower{aSpecularPower} {
  }
};

struct CommandSetFogParameters final : CommandT<CommandType::SetFogParameters> {
  Color color;
  f32 start;
  f32 end;
  f32 density;

  constexpr CommandSetFogParameters(Color const& aColor, f32 const aStart,
                                    f32 const aEnd, f32 const aDensity) noexcept
    : color{aColor}
    , start{aStart}
    , end{aEnd}
    , density{aDensity} {
  }
};

struct CommandSetReferenceAlpha final
  : CommandT<CommandType::SetReferenceAlpha> {
  u8 value;

  constexpr explicit CommandSetReferenceAlpha(u8 const aValue) noexcept
    : value{aValue} {
  }
};

struct CommandSetTextureFactor final : CommandT<CommandType::SetTextureFactor> {
  Color textureFactor;

  constexpr explicit CommandSetTextureFactor(
    Color const& aTextureFactor) noexcept
    : textureFactor{aTextureFactor} {
  }
};

struct CommandSetTextureStageConstant final
  : CommandT<CommandType::SetTextureStageConstant> {
  u8 stageId;
  Color constant;

  constexpr CommandSetTextureStageConstant(u8 const aStageId,
                                           Color const& aConstant) noexcept
    : stageId{aStageId}
    , constant{aConstant} {
  }
};

#define VISIT(cmdStruct)                                                       \
  case cmdStruct::TYPE:                                                        \
    visitor(cmd.as<cmdStruct>());                                              \
    return

template <typename Visitor>
auto visit(Command const& cmd, Visitor&& visitor) -> void {
  switch (cmd.type) {
    VISIT(CommandClearAttachments);
    VISIT(CommandDraw);
    VISIT(CommandDrawIndexed);
    VISIT(CommandBindPipeline);
    VISIT(CommandBindVertexBuffer);
    VISIT(CommandBindIndexBuffer);
    VISIT(CommandBindSampler);
    VISIT(CommandBindTexture);
    VISIT(CommandSetStencilReference);
    VISIT(CommandSetStencilReadMask);
    VISIT(CommandSetStencilWriteMask);
    VISIT(CommandSetBlendConstant);
    VISIT(CommandSetTransform);
    VISIT(CommandSetAmbientLight);
    VISIT(CommandSetLights);
    VISIT(CommandSetMaterial);
    VISIT(CommandSetFogParameters);
    VISIT(CommandSetReferenceAlpha);
    VISIT(CommandSetTextureFactor);
    VISIT(CommandSetTextureStageConstant);

  default:
    break;
  }

  visitor(cmd);
}

#undef VISIT

namespace ext {

struct CommandDrawXMesh final : CommandT<CommandType::ExtDrawXMesh> {
  XMesh xMeshId;

  constexpr explicit CommandDrawXMesh(XMesh const aXMeshId) noexcept
    : xMeshId{aXMeshId} {
  }
};

struct CommandRenderDearImGui final
  : CommandT<CommandType::ExtRenderDearImGui> {};

struct CommandBeginEffect final : CommandT<CommandType::ExtBeginEffect> {
  EffectId effect;

  constexpr explicit CommandBeginEffect(EffectId const aEffect) noexcept
    : effect{aEffect} {
  }
};

struct CommandEndEffect final : CommandT<CommandType::ExtEndEffect> {};

struct CommandBeginEffectPass final
  : CommandT<CommandType::ExtBeginEffectPass> {
  u32 passIndex;

  constexpr explicit CommandBeginEffectPass(u32 const aPassIndex) noexcept
    : passIndex{aPassIndex} {
  }
};

struct CommandEndEffectPass final : CommandT<CommandType::ExtEndEffectPass> {};

} // namespace ext

} // namespace basalt::gfx
