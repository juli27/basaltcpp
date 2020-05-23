#pragma once
#ifndef BASALT_RUNTIME_DEAR_IMGUI_H
#define BASALT_RUNTIME_DEAR_IMGUI_H

#include "runtime/shared/Types.h"

namespace basalt {

namespace gfx::backend {
struct IRenderer;
} // gfx::backend

struct DearImGui final {
  DearImGui();

  DearImGui(const DearImGui&) = delete;
  DearImGui(DearImGui&&) = delete;

  ~DearImGui();

  auto operator=(const DearImGui&) -> DearImGui& = delete;
  auto operator=(DearImGui&&) -> DearImGui& = delete;

  void new_frame(gfx::backend::IRenderer*, f64 deltaTime);
};

} //namespace basalt

#endif // BASALT_RUNTIME_DEAR_IMGUI_H
