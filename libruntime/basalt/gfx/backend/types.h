#pragma once

#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/base/types.h>

#include <memory>
#include <vector>

namespace basalt::gfx {

using Composite = std::vector<CommandList>;

struct Context;
using ContextPtr = std::shared_ptr<Context>;

struct CommandClearAttachments;
struct CommandDraw;
struct CommandBindPipeline;
struct CommandBindVertexBuffer;
struct CommandBindSampler;
struct CommandBindTexture;
struct CommandSetTransform;
struct CommandSetAmbientLight;
struct CommandSetLights;
struct CommandSetMaterial;

enum class CommandType : u8 {
  ClearAttachments,
  Draw,
  BindPipeline,
  BindVertexBuffer,
  BindSampler,
  BindTexture,

  // fixed function only
  SetTransform,
  SetAmbientLight,
  SetLights,
  SetMaterial,

  // built-in extensions
  ExtDrawXMesh,
  ExtRenderDearImGui,
};

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

namespace ext {

struct CommandDrawXMesh;
struct CommandRenderDearImGui;

} // namespace ext

} // namespace basalt::gfx
