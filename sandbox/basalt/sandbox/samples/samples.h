#pragma once

#include <basalt/api/types.h>

class Samples final {
public:
  static auto new_simple_view_rendering_sample(basalt::Engine&)
    -> basalt::ViewPtr;
  static auto new_simple_scene_sample(basalt::Engine&) -> basalt::ViewPtr;
  static auto new_dynamic_mesh_sample(basalt::Engine&) -> basalt::ViewPtr;
  static auto new_textures_sample(basalt::Engine&) -> basalt::ViewPtr;
  static auto new_fog_sample(basalt::Engine&) -> basalt::ViewPtr;
  static auto new_lighting_sample(basalt::Engine&) -> basalt::ViewPtr;

  static auto new_d3dx_x_mesh_sample(basalt::Engine&) -> basalt::ViewPtr;
};
