#pragma once

#include <basalt/gfx/backend/commands.h>

#include <basalt/api/gfx/backend/device.h>
#include <basalt/api/gfx/backend/ext/types.h>

#include <memory>
#include <optional>
#include <type_traits>

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

template <typename T>
auto query_device_extension(Device& device)
  -> std::optional<std::shared_ptr<T>> {
  static_assert(std::is_base_of_v<ext::Extension, T>);

  auto maybeExt {device.query_extension(T::ID)};
  if (!maybeExt) {
    return std::nullopt;
  }

  return std::static_pointer_cast<T>(maybeExt.value());
}

} // namespace basalt::gfx
