#pragma once

#include <basalt/api/types.h>

class TribaseExamples final {
public:
  static auto new_first_triangle_example(basalt::Engine&) -> basalt::ViewPtr;
  static auto new_02_04_textures_example(basalt::Engine&) -> basalt::ViewPtr;
  static auto new_02_04_textures_exercise1(basalt::Engine&) -> basalt::ViewPtr;
};
