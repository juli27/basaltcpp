#pragma once

#include "types.h"

#include <basalt/api/canvas.h>
#include <basalt/api/engine.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/shared/types.h>

#include <memory>

namespace basalt {

class Runtime final : public Engine {
public:
  Runtime(gfx::ContextPtr, std::unique_ptr<Canvas>);

  auto canvas() const -> Canvas const&;
  auto canvas() -> Canvas&;

  [[nodiscard]]
  auto dear_imgui() const -> DearImGuiPtr const&;

  struct UpdateContext final {
    SecondsF32 deltaTime;
  };

  auto update(UpdateContext const&) -> void;

private:
  std::unique_ptr<Canvas> mCanvas;
  DearImGuiPtr mDearImGui;
};

} // namespace basalt
