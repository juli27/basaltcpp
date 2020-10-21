#include "meshes.h"

#include <api/debug.h>
#include <api/engine.h>
#include <api/prelude.h>

#include <api/gfx/draw_target.h>
#include <api/gfx/backend/ext/types.h>

#include <api/scene/transform.h>

#include <api/shared/config.h>

using std::string_view;
using namespace std::literals;

using basalt::Debug;
using basalt::Engine;
using basalt::Transform;
using basalt::gfx::Model;
using basalt::gfx::SceneView;
using basalt::gfx::ext::XModel;

namespace d3d9 {

Meshes::Meshes(Engine& engine) {
  mScene->set_background(Colors::BLUE);
  mScene->set_ambient_light(Colors::WHITE);

  (void)mTiger.emplace<Transform>();
  mTiger.emplace<Model>(engine.load<XModel>("data/Tiger.x"sv));
}

void Meshes::on_update(const basalt::UpdateContext& ctx) {
  mTiger.get<Transform>().rotate(0.0f, static_cast<f32>(ctx.deltaTime), 0.0f);

  ctx.drawTarget.draw(mSceneView);

  if (ctx.engine.config().debugUiEnabled) {
    Debug::update(*mScene);
  }
}

auto Meshes::name() -> string_view {
  return "Tutorial 6: Using Meshes"sv;
}

} // namespace d3d9
