#pragma once

#include <basalt/gfx/backend/commands.h>

namespace basalt::gfx {

#define VISIT(cmdStruct)                                                       \
  case cmdStruct::TYPE:                                                        \
    visitor(cmd.as<cmdStruct>());                                              \
    break

template <typename Visitor>
auto visit(const Command& cmd, Visitor&& visitor) -> void {
  switch (cmd.type) {
    VISIT(CommandClearAttachments);
    VISIT(CommandDraw);
    VISIT(CommandBindPipeline);
    VISIT(CommandBindVertexBuffer);
    VISIT(CommandBindSampler);
    VISIT(CommandBindTexture);
    VISIT(CommandSetTransform);
    VISIT(CommandSetAmbientLight);
    VISIT(CommandSetLights);
    VISIT(CommandSetMaterial);
  default:
    visitor(cmd);
    break;
  }
}

#undef VISIT

} // namespace basalt::gfx
