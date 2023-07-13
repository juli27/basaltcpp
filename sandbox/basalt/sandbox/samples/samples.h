#pragma once

#include <basalt/api/types.h>

class Samples final {
public:
  static auto new_textures_sample(basalt::Engine&) -> basalt::ViewPtr;
  static auto new_simple_scene_sample(basalt::Engine&) -> basalt::ViewPtr;
  static auto new_cubes_sample(basalt::Engine&) -> basalt::ViewPtr;
};
