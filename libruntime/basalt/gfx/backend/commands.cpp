#include <basalt/gfx/backend/commands.h>

namespace basalt::gfx {

static_assert(sizeof(CommandClearAttachments) == 32);
static_assert(sizeof(CommandDraw) == 12);
static_assert(sizeof(CommandDrawIndexed) == 24);
static_assert(sizeof(CommandBindPipeline) == 8);
static_assert(sizeof(CommandBindVertexBuffer) == 16);
static_assert(sizeof(CommandBindIndexBuffer) == 8);
static_assert(sizeof(CommandBindSampler) == 8);
static_assert(sizeof(CommandBindTexture) == 8);
static_assert(sizeof(CommandSetBlendConstant) == 20);
static_assert(sizeof(CommandSetTransform) == 68);
static_assert(sizeof(CommandSetAmbientLight) == 20);
static_assert(sizeof(CommandSetLights) == 24);
static_assert(sizeof(CommandSetMaterial) == 72);
static_assert(sizeof(CommandSetFogParameters) == 32);
static_assert(sizeof(CommandSetReferenceAlpha) == 2);
static_assert(sizeof(CommandSetTextureFactor) == 20);
static_assert(sizeof(CommandSetTextureStageConstant) == 20);

namespace ext {

static_assert(sizeof(CommandDrawXMesh) == 8);
static_assert(sizeof(CommandRenderDearImGui) == 1);
static_assert(sizeof(CommandBeginEffect) == 8);
static_assert(sizeof(CommandEndEffect) == 1);
static_assert(sizeof(CommandBeginEffectPass) == 8);
static_assert(sizeof(CommandEndEffectPass) == 1);

} // namespace ext

} // namespace basalt::gfx
