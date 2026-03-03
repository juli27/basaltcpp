#include <basalt/gfx/backend/ext/dear_imgui_renderer.h>

#include <basalt/gfx/backend/commands.h>
#include <basalt/gfx/backend/command_list_p.h>

namespace basalt::gfx::ext {

auto DearImGuiCommandEncoder::render_dear_imgui(CommandList& cmdList) -> void {
  CommandListP::add<CommandRenderDearImGui>(cmdList);
}

} // namespace basalt::gfx::ext
