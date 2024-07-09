#pragma once

#include <basalt/types.h>

#include <basalt/api/engine.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/shared/types.h>

namespace basalt {

class Runtime final : public Engine {
public:
  Runtime(Config, gfx::ContextPtr);

  [[nodiscard]]
  auto dear_imgui() const -> DearImGuiPtr const&;

  [[nodiscard]]
  auto is_dirty() const noexcept -> bool;

  auto set_dirty(bool) noexcept -> void;

  struct UpdateContext final {
    SecondsF32 deltaTime;
  };

  auto update(UpdateContext const&) -> void;

private:
  DearImGuiPtr mDearImGui;
};

} // namespace basalt
