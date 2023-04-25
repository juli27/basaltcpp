#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <basalt/gfx/backend/commands.h>
#include <basalt/gfx/backend/command_list.h>

namespace basalt::gfx::ext {

auto XMeshCommandEncoder::draw_x_mesh(CommandList& cmdList, const XMesh meshId,
                                      const u32 subset) -> void {
  cmdList.add<CommandDrawXMesh>(meshId, subset);
}

} // namespace basalt::gfx::ext
