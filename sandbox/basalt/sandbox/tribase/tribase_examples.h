#pragma once

#include <basalt/api/types.h>

class TribaseExamples final {
public:
  static auto new_first_triangle_example(basalt::Engine&) -> basalt::ViewPtr;
  static auto new_02_04_textures_example(basalt::Engine&) -> basalt::ViewPtr;
  static auto new_02_04_textures_exercises(basalt::Engine&) -> basalt::ViewPtr;
  static auto new_02_05_buffers_example(basalt::Engine&) -> basalt::ViewPtr;
  static auto new_02_05_buffers_exercises(basalt::Engine&) -> basalt::ViewPtr;
  static auto new_02_06_fog_example(basalt::Engine&) -> basalt::ViewPtr;
  static auto new_02_07_lighting_example(basalt::Engine&) -> basalt::ViewPtr;
};
