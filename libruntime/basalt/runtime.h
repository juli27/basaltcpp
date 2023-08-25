#pragma once

#include <basalt/types.h>

#include <basalt/gfx/backend/types.h>

#include <basalt/api/engine.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/shared/types.h>

namespace basalt {

class Runtime : public Engine {
public:
  [[nodiscard]] auto dear_imgui() const -> const DearImGuiPtr&;

  struct UpdateContext final {
    SecondsF32 deltaTime;
  };

  auto update(const UpdateContext&) -> void;

protected:
  Runtime(Config&, gfx::Info, gfx::SwapChainPtr, DearImGuiPtr);

private:
  gfx::SwapChainPtr mSwapChain;
  DearImGuiPtr mDearImGui;
};

} // namespace basalt
