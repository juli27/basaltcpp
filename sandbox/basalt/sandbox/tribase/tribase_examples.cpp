#include <basalt/sandbox/tribase/tribase_examples.h>

#include <basalt/sandbox/tribase/02-03_first_triangle.h>
#include <basalt/sandbox/tribase/02-04_textures.h>
#include <basalt/sandbox/tribase/02-04_textures_exercises.h>

#include <memory>

auto TribaseExamples::new_first_triangle_example(basalt::Engine& engine)
  -> basalt::ViewPtr {
  return std::make_shared<tribase::FirstTriangle>(engine);
}

auto TribaseExamples::new_02_04_textures_example(basalt::Engine& engine)
  -> basalt::ViewPtr {
  return std::make_shared<tribase::Textures>(engine);
}

auto TribaseExamples::new_02_04_textures_exercises(basalt::Engine& engine)
  -> basalt::ViewPtr {
  return std::make_shared<tribase::TexturesExercises>(engine);
}
