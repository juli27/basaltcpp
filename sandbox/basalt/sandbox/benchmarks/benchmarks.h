#pragma once

#include <basalt/api/types.h>

class Benchmarks {
public:
  static auto make_textured_triangles_view(basalt::Engine&) -> basalt::ViewPtr;
  static auto make_cubes_view(basalt::Engine&) -> basalt::ViewPtr;
};
