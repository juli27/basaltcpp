#include <basalt/sandbox/d3d9/meshes.h>

#include <basalt/api/debug.h>
#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/api/scene/transform.h>

#include <basalt/api/shared/config.h>

using namespace std::literals;

using namespace entt::literals;

using basalt::Debug;
using basalt::Engine;
using basalt::Transform;
using basalt::gfx::ext::XModel;

namespace d3d9 {

Meshes::Meshes(Engine& engine) {
  mScene->set_background(Colors::BLUE);
  mScene->set_ambient_light(Colors::WHITE);

  mTiger.emplace<XModel>(engine.get_or_load<XModel>("data/Tiger.x"_hs));
}

auto Meshes::drawable() -> basalt::gfx::DrawablePtr {
  return mSceneView;
}

void Meshes::on_tick(Engine& engine) {
  mTiger.get<Transform>().rotate(0.0f, static_cast<f32>(engine.delta_time()),
                                 0.0f);

  if (engine.config().get_bool("runtime.debugUI.enabled"s)) {
    Debug::update(*mScene);
  }
}

} // namespace d3d9
