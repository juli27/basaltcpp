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

  auto tick() -> void;

  auto render() -> void;

protected:
  Runtime(Config&, gfx::Info, gfx::ContextPtr, DearImGuiPtr);

private:
  gfx::ContextPtr mGfxContext;
  DearImGuiPtr mDearImGui;

  [[nodiscard]] auto draw() -> gfx::Composite;
};

} // namespace basalt
