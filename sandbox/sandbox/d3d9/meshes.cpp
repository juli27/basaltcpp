#include "meshes.h"

#include "utils.h"

#include <runtime/debug.h>
#include <runtime/prelude.h>

#include <runtime/gfx/draw_target.h>
#include <runtime/scene/transform.h>
#include <runtime/shared/config.h>

using std::string_view;
using namespace std::literals;

using basalt::Debug;
using basalt::Transform;
using basalt::gfx::Device;
using basalt::gfx::RenderComponent;
using basalt::gfx::SceneView;

namespace d3d9 {

Meshes::Meshes(Device& renderer) {
  mScene->set_background_color(Colors::BLUE);
  mScene->set_ambient_light(Colors::WHITE);

  entt::registry& ecs {mScene->ecs()};
  mTiger = ecs.create();
  ecs.emplace<Transform>(mTiger);

  auto& rc {ecs.emplace<RenderComponent>(mTiger)};
  rc.model = renderer.load_model("data/Tiger.x");

  mSceneView = std::make_shared<SceneView>(mScene, create_default_camera());
}

void Meshes::on_update(const basalt::UpdateContext& ctx) {
  auto& transform {mScene->ecs().get<Transform>(mTiger)};
  transform.rotate(0.0f, static_cast<f32>(ctx.deltaTime), 0.0f);

  ctx.drawTarget.draw(mSceneView);

  if (ctx.engine.config().debugUiEnabled) {
      Debug::update(*mScene);
  }
}

auto Meshes::name() -> string_view {
  return "Tutorial 6: Using Meshes"sv;
}

} // namespace d3d9
