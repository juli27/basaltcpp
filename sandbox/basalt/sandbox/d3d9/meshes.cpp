#include <basalt/sandbox/d3d9/meshes.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/api/scene/transform.h>

using namespace entt::literals;

using basalt::Engine;
using basalt::Transform;
using basalt::gfx::ext::XModel;

namespace d3d9 {

Meshes::Meshes(Engine& engine) {
  add_child_top(mSceneView);

  mScene->set_background(Colors::BLUE);
  mScene->set_ambient_light(Colors::WHITE);

  mTiger.emplace<XModel>(engine.get_or_load<XModel>("data/Tiger.x"_hs));
}

void Meshes::on_tick(Engine& engine) {
  mTiger.get<Transform>().rotate(0.0f, static_cast<f32>(engine.delta_time()),
                                 0.0f);
}

} // namespace d3d9
