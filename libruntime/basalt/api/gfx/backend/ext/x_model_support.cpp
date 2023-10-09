#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <basalt/gfx/backend/commands.h>
#include <basalt/gfx/backend/command_list.h>

namespace basalt::gfx::ext {

auto XMeshCommandEncoder::draw_x_mesh(CommandList& cmdList, XMesh const meshId)
  -> void {
  cmdList.add<CommandDrawXMesh>(meshId);
}

} // namespace basalt::gfx::ext
