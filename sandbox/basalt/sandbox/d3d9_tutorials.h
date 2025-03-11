#pragma once

#include <basalt/api/types.h>

class D3D9Tutorials {
public:
  static auto new_textures_tutorial(basalt::Engine&) -> basalt::ViewPtr;
  static auto new_meshes_tutorial(basalt::Engine&) -> basalt::ViewPtr;
};
