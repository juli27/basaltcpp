#pragma once

#include <basalt/api/types.h>

class TribaseExamples {
public:
  static auto new_02_05_buffers_exercises(basalt::Engine&) -> basalt::ViewPtr;
  static auto new_02_06_fog_example(basalt::Engine&) -> basalt::ViewPtr;
  static auto new_02_07_lighting_example(basalt::Engine&) -> basalt::ViewPtr;
  static auto new_02_08_blending_example(basalt::Engine&) -> basalt::ViewPtr;
  static auto new_02_09_multi_texturing_example(basalt::Engine&)
    -> basalt::ViewPtr;
  static auto new_02_10_volume_textures(basalt::Engine&) -> basalt::ViewPtr;
  static auto new_02_11_env_mapping(basalt::Engine&) -> basalt::ViewPtr;
  static auto new_02_12_bump_mapping(basalt::Engine&) -> basalt::ViewPtr;
  static auto new_02_13_stencil_buffer(basalt::Engine&) -> basalt::ViewPtr;
  static auto new_02_14_effects(basalt::Engine&) -> basalt::ViewPtr;
};
