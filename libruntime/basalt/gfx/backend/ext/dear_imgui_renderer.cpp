#include <basalt/gfx/backend/ext/dear_imgui_renderer.h>

#include <basalt/gfx/backend/commands.h>
#include <basalt/gfx/backend/command_list.h>

namespace basalt::gfx::ext {

auto DearImGuiCommandEncoder::render_dear_imgui(CommandList& cmdList) -> void {
  cmdList.add<CommandRenderDearImGui>();
}

} // namespace basalt::gfx::ext
