#pragma once
#ifndef BASALT_RUNTIME_DEAR_IMGUI_H
#define BASALT_RUNTIME_DEAR_IMGUI_H

namespace basalt {

namespace gfx::backend {
struct IRenderer;
} // gfx::backend

struct DearImGui final {
  static void init();
  static void shutdown();
  static void new_frame(gfx::backend::IRenderer*);
};

} //namespace basalt

#endif // BASALT_RUNTIME_DEAR_IMGUI_H
