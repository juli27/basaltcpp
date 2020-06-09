#include "sandbox/d3d9/device.h"

#include "utils.h"

#include <runtime/debug.h>
#include <runtime/prelude.h>

using namespace std::literals;

using std::string_view;

using basalt::Debug;
using basalt::gfx::SceneView;

namespace d3d9 {

Device::Device() {
  mScene->set_background_color(Colors::BLUE);
  mSceneView = std::make_shared<SceneView>(mScene, create_default_camera());
}

void Device::on_update(const basalt::UpdateContext& ctx) {
  ctx.drawTarget.draw(mSceneView);

  if (ctx.engine.config().debugUiEnabled) {
      Debug::update(*mScene);
  }
}

auto Device::name() -> string_view {
  return "Tutorial 1: Creating a Device"sv;
}

} // namespace d3d9
