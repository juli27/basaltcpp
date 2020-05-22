#include "sandbox/d3d9/meshes.h"

#include "sandbox/d3d9/utils.h"

#include <runtime/Engine.h>
#include <runtime/Prelude.h>

#include <runtime/gfx/types.h>
#include <runtime/scene/transform.h>

using namespace std::literals;

using basalt::Transform;
using basalt::gfx::RenderComponent;

namespace d3d9 {

Meshes::Meshes(basalt::gfx::backend::IRenderer* const renderer) {
  mScene->set_background_color(Color {0.0f, 0.0f, 1.0f});
  mScene->set_ambient_light(Color {1.0f, 1.0f, 1.0f});

  const auto [entity, transform, rc] = mScene->create_entity<Transform,
    RenderComponent>();
  mTiger = entity;

  rc.model = renderer->load_model("data/Tiger.x");
}

void Meshes::on_show() {
  basalt::set_view({mScene, create_default_camera()});
}

void Meshes::on_hide() {
}

void Meshes::on_update(const f64 deltaTime) {
  auto& transform = mScene->get_entity_registry().get<Transform>(mTiger);
  transform.rotate(0.0f, static_cast<f32>(deltaTime), 0.0f);
}

auto Meshes::name() -> std::string_view {
  return "Tutorial 6: Using Meshes"sv;
}

} // namespace d3d9
