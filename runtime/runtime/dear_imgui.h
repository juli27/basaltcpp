#pragma once
#ifndef BASALT_RUNTIME_DEAR_IMGUI_H
#define BASALT_RUNTIME_DEAR_IMGUI_H

namespace basalt {

struct DearImGui final {
  static void init();
  static void shutdown();
  static void new_frame();
};

} //namespace basalt

#endif // BASALT_RUNTIME_DEAR_IMGUI_H
