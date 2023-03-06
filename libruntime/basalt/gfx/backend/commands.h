#pragma once

#include <basalt/gfx/backend/types.h>

#include <basalt/api/gfx/backend/types.h>
#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/api/math/matrix4x4.h>

#include <basalt/api/shared/asserts.h>
#include <basalt/api/shared/color.h>

#include <basalt/api/base/types.h>

#include <gsl/span>

namespace basalt::gfx {

struct Command {
  const CommandType type;

  template <typename T>
  auto as() const -> const T& {
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

  constexpr explicit CommandClearAttachments(const Attachments aAttachments,
                                             const Color& aColor,
                                             const f32 aDepth,
                                             const u32 aStencil) noexcept
    : attachments {aAttachments}
    , color {aColor}
    , depth {aDepth}
    , stencil {aStencil} {
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

struct CommandDrawIndexed final : CommandT<CommandType::DrawIndexed> {
  i32 vertexOffset {};
  u32 minIndex {};
  u32 numVertices {};
  u32 firstIndex {};
  u32 indexCount {};

  constexpr CommandDrawIndexed(const i32 aVertexOffset, const u32 aMinIndex,
                               const u32 aNumVertices, const u32 aFirstIndex,
                               const u32 aPrimitiveCount) noexcept
    : vertexOffset {aVertexOffset}
    , minIndex {aMinIndex}
    , numVertices {aNumVertices}
    , firstIndex {aFirstIndex}
    , indexCount {aPrimitiveCount} {
  }
};

struct CommandBindPipeline final : CommandT<CommandType::BindPipeline> {
  Pipeline handle;

  constexpr explicit CommandBindPipeline(const Pipeline pipeline) noexcept
    : handle {pipeline} {
  }
};

static_assert(sizeof(CommandBindPipeline) == 8);

struct CommandBindVertexBuffer final : CommandT<CommandType::BindVertexBuffer> {
  VertexBuffer handle;
  uDeviceSize offset;

  constexpr explicit CommandBindVertexBuffer(const VertexBuffer vb,
                                             const uDeviceSize aOffset) noexcept
    : handle {vb}, offset {aOffset} {
  }
};

static_assert(sizeof(CommandBindVertexBuffer) == 16);

struct CommandBindIndexBuffer final : CommandT<CommandType::BindIndexBuffer> {
  IndexBuffer handle;

  constexpr explicit CommandBindIndexBuffer(const IndexBuffer vb) noexcept
    : handle {vb} {
  }
};

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
  Matrix4x4f32 transform;

  CommandSetTransform(const TransformState s, const Matrix4x4f32& t) noexcept
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

namespace ext {

struct CommandDrawXMesh final : CommandT<CommandType::ExtDrawXMesh> {
  XMesh handle;
  u32 subset;

  constexpr CommandDrawXMesh(const XMesh meshHandle,
                             const u32 subsetIndex) noexcept
    : handle {meshHandle}, subset {subsetIndex} {
  }
};

static_assert(sizeof(CommandDrawXMesh) == 12);

struct CommandRenderDearImGui final
  : CommandT<CommandType::ExtRenderDearImGui> {};

static_assert(sizeof(CommandRenderDearImGui) == 1);

} // namespace ext

} // namespace basalt::gfx
