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
static_assert(sizeof(CommandSetTransform) == 68);
static_assert(sizeof(CommandSetAmbientLight) == 20);
static_assert(sizeof(CommandSetLights) == 24);
static_assert(sizeof(CommandSetMaterial) == 72);
static_assert(sizeof(CommandSetFogParameters) == 32);

namespace ext {

static_assert(sizeof(CommandDrawXMesh) == 12);
static_assert(sizeof(CommandRenderDearImGui) == 1);

} // namespace ext

} // namespace basalt::gfx
