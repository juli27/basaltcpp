#include "runtime/gfx/backend/render_command.h"

using std::vector;

namespace basalt::gfx::backend {

using math::Mat4f32;

RenderCommandList::RenderCommandList(
  const Mat4f32& view, const Mat4f32& projection
)
  : mView(view), mProjection(projection) {
}

auto RenderCommandList::commands() const -> const vector<RenderCommand>& {
  return mCommands;
}

auto RenderCommandList::view() const -> const Mat4f32& {
  return mView;
}

auto RenderCommandList::projection() const -> const Mat4f32& {
  return mProjection;
}

auto RenderCommandList::ambient_light() const -> const Color& {
  return mAmbientLightColor;
}

void RenderCommandList::set_ambient_light(const Color& color) {
  mAmbientLightColor = color;
}

auto RenderCommandList::directional_lights() const -> const std::vector<
  DirectionalLight>& {
  return mDirectionalLights;
}

void RenderCommandList::set_directional_lights(
  const std::vector<DirectionalLight>& lights
) {
  mDirectionalLights = lights;
}

void RenderCommandList::add(const RenderCommand& command) {
  mCommands.push_back(command);
}

} // namespace basalt::gfx::backend
