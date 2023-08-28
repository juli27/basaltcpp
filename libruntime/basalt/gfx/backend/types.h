#pragma once

#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/base/types.h>

#include <memory>
#include <vector>

namespace basalt::gfx {

using Composite = std::vector<CommandList>;

class ValidatingDevice;
using ValidatingDevicePtr = std::shared_ptr<ValidatingDevice>;

class ValidatingSwapChain;
using ValidatingSwapChainPtr = std::shared_ptr<ValidatingSwapChain>;

struct CommandClearAttachments;
struct CommandDraw;
struct CommandDrawIndexed;
struct CommandBindPipeline;
struct CommandBindVertexBuffer;
struct CommandBindIndexBuffer;
struct CommandBindSampler;
struct CommandBindTexture;
struct CommandSetStencilReference;
struct CommandSetStencilReadMask;
struct CommandSetStencilWriteMask;
struct CommandSetBlendConstant;
struct CommandSetTransform;
struct CommandSetAmbientLight;
struct CommandSetLights;
struct CommandSetMaterial;
struct CommandSetFogParameters;
struct CommandSetReferenceAlpha;
struct CommandSetTextureFactor;
struct CommandSetTextureStageConstant;

enum class CommandType : u8;

enum class DeviceStatus : u8 {
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
