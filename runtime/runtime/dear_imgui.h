#pragma once
#ifndef BASALT_RUNTIME_DEAR_IMGUI_H
#define BASALT_RUNTIME_DEAR_IMGUI_H

#include "types.h"

namespace basalt {

namespace gfx::backend {
struct IRenderer;
} // gfx::backend

struct DearImGui final {
  explicit DearImGui(gfx::backend::IRenderer*);

  DearImGui(const DearImGui&) = delete;
  DearImGui(DearImGui&&) = delete;

  ~DearImGui();

  auto operator=(const DearImGui&) -> DearImGui& = delete;
  auto operator=(DearImGui&&) -> DearImGui& = delete;

  void new_frame(const UpdateContext&);

private:
  gfx::backend::IRenderer* mRenderer {};
};

} //namespace basalt

#endif // BASALT_RUNTIME_DEAR_IMGUI_H
