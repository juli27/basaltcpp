#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/gfx/backend/commands.h>
#include <basalt/api/gfx/backend/ext/dear_imgui_renderer.h>
#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <algorithm>
#include <array>
#include <utility>

using std::array;
using std::vector;

namespace basalt::gfx {

namespace {

// 128 KiB
constexpr uSize INITIAL_COMMAND_BUFFER_SIZE {128 * 1024};

} // namespace

CommandList::CommandList()
  : mBuffer {std::make_unique<CommandBuffer>(INITIAL_COMMAND_BUFFER_SIZE)} {
}

auto CommandList::commands() const noexcept -> const vector<Command*>& {
  return mCommands;
}

void CommandList::clear(const Color& color) {
  add<CommandClear>(color);
}

void CommandList::draw(const VertexBuffer vertexBuffer, const u32 startVertex,
                       const PrimitiveType primitiveType,
                       const u32 primitiveCount) {
  add<CommandDraw>(vertexBuffer, primitiveType, startVertex, primitiveCount);
}

void CommandList::set_directional_lights(
  const vector<DirectionalLight>& lights) {
  BASALT_ASSERT(lights.size() <= 4);

  array<DirectionalLight, 4> directionalLights {};
  std::copy_n(lights.begin(), std::min(lights.size(), directionalLights.size()),
              directionalLights.begin());

  add<CommandSetDirectionalLights>(directionalLights);
}

void CommandList::set_transform(const TransformState state,
                                const Mat4f32& transform) {
  add<CommandSetTransform>(state, transform);
}

void CommandList::set_material(const Color& diffuse, const Color& ambient,
                               const Color& emissive) {
  add<CommandSetMaterial>(diffuse, ambient, emissive);
}

void CommandList::set_render_state(const RenderState& renderState) {
  add<CommandSetRenderState>(renderState);
}

void CommandList::bind_texture(const Texture texture) {
  add<CommandBindTexture>(texture);
}

void CommandList::bind_sampler(const Sampler sampler) {
  add<CommandBindSampler>(sampler);
}

void CommandList::set_texture_stage_state(const u8 stage,
                                          const TextureStageState state,
                                          const u32 value) {
  add<CommandSetTextureStageState>(stage, state, value);
}

void CommandList::ext_draw_x_model(const ext::XModel handle) {
  add<ext::CommandDrawXModel>(handle);
}

void CommandList::ext_render_dear_imgui() {
  add<ext::CommandRenderDearImGui>();
}

} // namespace basalt::gfx
