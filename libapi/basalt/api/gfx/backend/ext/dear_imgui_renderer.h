#pragma once

#include <basalt/api/gfx/backend/ext/extension.h>

#include <basalt/api/gfx/backend/types.h>

namespace basalt::gfx::ext {

struct DearImGuiRenderer : ExtensionT<ExtensionId::DearImGuiRenderer> {
  virtual void init() = 0;
  virtual void shutdown() = 0;
  virtual void new_frame() = 0;

protected:
  DearImGuiRenderer() = default;
};

struct CommandRenderDearImGui final
  : CommandT<CommandType::ExtRenderDearImGui> {};

static_assert(sizeof(CommandRenderDearImGui) == 1);

} // namespace basalt::gfx::ext
