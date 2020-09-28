#pragma once

#include "extension.h"

#include "runtime/gfx/backend/types.h"

namespace basalt::gfx::ext {

struct DearImGuiRenderer : Extension {
  DearImGuiRenderer(const DearImGuiRenderer&) = delete;
  DearImGuiRenderer(DearImGuiRenderer&&) = delete;

  virtual ~DearImGuiRenderer() = default;

  auto operator=(const DearImGuiRenderer&) -> DearImGuiRenderer& = delete;
  auto operator=(DearImGuiRenderer&&) -> DearImGuiRenderer& = delete;

  virtual void init() = 0;
  virtual void shutdown() = 0;
  virtual void new_frame() = 0;

protected:
  DearImGuiRenderer() = default;
};

struct CommandRenderImGui final : CommandT<CommandType {128}> {
  CommandRenderImGui() noexcept = default;

  CommandRenderImGui(const CommandRenderImGui&) = default;
  CommandRenderImGui(CommandRenderImGui&&) = default;

  ~CommandRenderImGui() noexcept = default;

  auto operator=(const CommandRenderImGui&) -> CommandRenderImGui& = default;
  auto operator=(CommandRenderImGui&&) -> CommandRenderImGui& = default;
};

static_assert(sizeof(CommandRenderImGui) == 1);
static_assert(std::is_trivially_destructible_v<CommandRenderImGui>);

} // basalt::gfx::ext
