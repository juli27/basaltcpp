#pragma once
#ifndef BASALT_RUNTIME_DEAR_IMGUI_H
#define BASALT_RUNTIME_DEAR_IMGUI_H

#include "runtime/shared/Types.h"

namespace basalt {

namespace gfx::backend {
struct IRenderer;
} // gfx::backend

struct DearImGui final {
  static void init();
  static void shutdown();
  static void new_frame(gfx::backend::IRenderer*, f64 deltaTime);
};

} //namespace basalt

#endif // BASALT_RUNTIME_DEAR_IMGUI_H
