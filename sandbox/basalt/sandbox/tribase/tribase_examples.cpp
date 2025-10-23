#include <basalt/sandbox/tribase/tribase_examples.h>

#include <basalt/sandbox/tribase/02-05_buffers_exercises.h>
#include <basalt/sandbox/tribase/02-06_fog.h>
#include <basalt/sandbox/tribase/02-07_lighting.h>
#include <basalt/sandbox/tribase/02-08_blending.h>
#include <basalt/sandbox/tribase/02-09_multi_texturing.h>
#include <basalt/sandbox/tribase/02-10_volume_textures.h>
#include <basalt/sandbox/tribase/02-11_env_mapping.h>
#include <basalt/sandbox/tribase/02-12_bump_mapping.h>
#include <basalt/sandbox/tribase/02-13_stencil_buffer.h>
#include <basalt/sandbox/tribase/02-14_effects.h>

#include <memory>

auto TribaseExamples::new_02_05_buffers_exercises(basalt::Engine& engine)
  -> basalt::ViewPtr {
  return std::make_shared<tribase::BuffersExercises>(engine);
}

auto TribaseExamples::new_02_06_fog_example(basalt::Engine& engine)
  -> basalt::ViewPtr {
  return std::make_shared<tribase::Fog>(engine);
}

auto TribaseExamples::new_02_07_lighting_example(basalt::Engine& engine)
  -> basalt::ViewPtr {
  return std::make_shared<tribase::Lighting>(engine);
}

auto TribaseExamples::new_02_08_blending_example(basalt::Engine& engine)
  -> basalt::ViewPtr {
  return std::make_shared<tribase::Blending>(engine);
}

auto TribaseExamples::new_02_09_multi_texturing_example(basalt::Engine& engine)
  -> basalt::ViewPtr {
  return std::make_shared<tribase::MultiTexturing>(engine);
}

auto TribaseExamples::new_02_10_volume_textures(basalt::Engine& engine)
  -> basalt::ViewPtr {
  return std::make_shared<tribase::VolumeTextures>(engine);
}

auto TribaseExamples::new_02_11_env_mapping(basalt::Engine& engine)
  -> basalt::ViewPtr {
  return std::make_shared<tribase::EnvMapping>(engine);
}

auto TribaseExamples::new_02_12_bump_mapping(basalt::Engine& engine)
  -> basalt::ViewPtr {
  return std::make_shared<tribase::BumpMapping>(engine);
}

auto TribaseExamples::new_02_13_stencil_buffer(basalt::Engine& engine)
  -> basalt::ViewPtr {
  return std::make_shared<tribase::StencilBuffer>(engine);
}

auto TribaseExamples::new_02_14_effects(basalt::Engine& engine)
  -> basalt::ViewPtr {
  return std::make_shared<tribase::Effects>(engine);
}
