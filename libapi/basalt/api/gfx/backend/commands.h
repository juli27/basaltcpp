#pragma once

#include <basalt/api/gfx/backend/render_state.h>
#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/scene/types.h>

#include <basalt/api/math/mat4.h>

#include <basalt/api/shared/color.h>

#include <basalt/api/base/types.h>

#include <gsl/span>

#include <utility>

namespace basalt::gfx {

struct CommandClearAttachments final : CommandT<CommandType::ClearAttachments> {
  Attachments attachments;
  Color color;
  f32 z;
  u32 stencil;

  constexpr explicit CommandClearAttachments(const Attachments aAttachments,
                                             const Color& aColor, const f32 aZ,
                                             const u32 aStencil) noexcept
    : attachments {aAttachments}, color {aColor}, z {aZ}, stencil {aStencil} {
  }
};

static_assert(sizeof(CommandClearAttachments) == 32);

struct CommandDraw final : CommandT<CommandType::Draw> {
  u32 firstVertex {};
  u32 vertexCount {};

  constexpr CommandDraw(const u32 s, const u32 c) noexcept
    : firstVertex {s}, vertexCount {c} {
  }
};

static_assert(sizeof(CommandDraw) == 12);

struct CommandSetRenderState final : CommandT<CommandType::SetRenderState> {
  RenderState renderState;

  constexpr explicit CommandSetRenderState(RenderState state) noexcept
    : renderState {std::move(state)} {
  }
};

static_assert(sizeof(CommandSetRenderState) == 4);

struct CommandBindPipeline final : CommandT<CommandType::BindPipeline> {
  Pipeline handle;

  constexpr explicit CommandBindPipeline(const Pipeline pipeline) noexcept
    : handle {pipeline} {
  }
};

static_assert(sizeof(CommandBindPipeline) == 8);

struct CommandBindVertexBuffer final : CommandT<CommandType::BindVertexBuffer> {
  VertexBuffer handle;
  u64 offset;

  constexpr explicit CommandBindVertexBuffer(const VertexBuffer vb,
                                             const u64 aOffset) noexcept
    : handle {vb}, offset {aOffset} {
  }
};

static_assert(sizeof(CommandBindVertexBuffer) == 16);

struct CommandBindSampler final : CommandT<CommandType::BindSampler> {
  Sampler sampler {Sampler::null()};

  constexpr explicit CommandBindSampler(const Sampler s) noexcept
    : sampler {s} {
  }
};

static_assert(sizeof(CommandBindSampler) == 8);

struct CommandBindTexture final : CommandT<CommandType::BindTexture> {
  Texture texture {Texture::null()};

  constexpr explicit CommandBindTexture(const Texture t) noexcept
    : texture {t} {
  }
};

static_assert(sizeof(CommandBindTexture) == 8);

struct CommandSetTransform final : CommandT<CommandType::SetTransform> {
  TransformState state;
  Mat4f32 transform;

  CommandSetTransform(const TransformState s, const Mat4f32& t) noexcept
    : state {s}, transform {t} {
  }
};

static_assert(sizeof(CommandSetTransform) == 68);

struct CommandSetAmbientLight final : CommandT<CommandType::SetAmbientLight> {
  Color ambientColor;

  explicit CommandSetAmbientLight(const Color& c) noexcept : ambientColor {c} {
  }
};

static_assert(sizeof(CommandSetAmbientLight) == 20);

struct CommandSetLights final : CommandT<CommandType::SetLights> {
  gsl::span<const Light> lights;

  constexpr explicit CommandSetLights(const gsl::span<const Light> dl) noexcept
    : lights {dl} {
  }
};

static_assert(sizeof(CommandSetLights) == 24);

struct CommandSetMaterial final : CommandT<CommandType::SetMaterial> {
  Color diffuse;
  Color ambient;
  Color emissive;

  CommandSetMaterial(const Color& d, const Color& a, const Color& e) noexcept
    : diffuse {d}, ambient {a}, emissive {e} {
  }
};

static_assert(sizeof(CommandSetMaterial) == 52);

struct CommandSetTextureStageState final
  : CommandT<CommandType::SetTextureStageState> {
  u8 stage;
  TextureStageState state;
  u32 value;

  constexpr CommandSetTextureStageState(
    const u8 textureStage, const TextureStageState textureStageState,
    const u32 stateValue) noexcept
    : stage {textureStage}, state {textureStageState}, value {stateValue} {
  }
};

static_assert(sizeof(CommandSetTextureStageState) == 8);

} // namespace basalt::gfx
