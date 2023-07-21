#include <basalt/sandbox/samples/samples.h>

#include <basalt/sandbox/samples/cubes.h>
#include <basalt/sandbox/samples/lighting.h>
#include <basalt/sandbox/samples/simple_scene.h>
#include <basalt/sandbox/samples/textures.h>

#include <memory>

using basalt::Engine;
using basalt::ViewPtr;

auto Samples::new_textures_sample(Engine& engine) -> ViewPtr {
  return std::make_shared<samples::Textures>(engine);
}

auto Samples::new_simple_scene_sample(Engine& engine) -> ViewPtr {
  return std::make_shared<samples::SimpleScene>(engine);
}

auto Samples::new_cubes_sample(Engine& engine) -> ViewPtr {
  return std::make_shared<samples::Cubes>(engine);
}

auto Samples::new_lighting_sample(Engine& engine) -> ViewPtr {
  return std::make_shared<samples::Lighting>(engine);
}
