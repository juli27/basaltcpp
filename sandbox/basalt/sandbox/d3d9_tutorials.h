#pragma once

#include <basalt/api/types.h>

class D3D9Tutorials final {
public:
  static auto new_vertices_tutorial(basalt::Engine&) -> basalt::ViewPtr;
  static auto new_matrices_tutorial(basalt::Engine&) -> basalt::ViewPtr;
  static auto new_lights_tutorial(basalt::Engine&) -> basalt::ViewPtr;
  static auto new_textures_tutorial(basalt::Engine&) -> basalt::ViewPtr;
  static auto new_meshes_tutorial(basalt::Engine&) -> basalt::ViewPtr;
};
