#include "sandbox/d3d9/meshes.h"

#include "sandbox/d3d9/utils.h"

#include <runtime/Engine.h>
#include <runtime/prelude.h>

#include <runtime/scene/transform.h>

using std::string_view;
using namespace std::literals;

using basalt::Transform;
using basalt::gfx::RenderComponent;
using basalt::gfx::View;
using basalt::gfx::backend::IRenderer;

namespace d3d9 {

Meshes::Meshes(IRenderer* const renderer) {
  mScene->set_background_color(Color {0.0f, 0.0f, 1.0f});
  mScene->set_ambient_light(Color {1.0f, 1.0f, 1.0f});

  entt::registry& ecs {mScene->ecs()};
  mTiger = ecs.create();
  ecs.emplace<Transform>(mTiger);

  auto& rc {ecs.emplace<RenderComponent>(mTiger)};
  rc.model = renderer->load_model("data/Tiger.x");
}

void Meshes::on_show() {
  basalt::set_view(View {mScene, create_default_camera()});
}

void Meshes::on_hide() {
}

void Meshes::on_update(const f64 deltaTime) {
  auto& transform {mScene->ecs().get<Transform>(mTiger)};
  transform.rotate(0.0f, static_cast<f32>(deltaTime), 0.0f);
}

auto Meshes::name() -> string_view {
  return "Tutorial 6: Using Meshes"sv;
}

} // namespace d3d9
