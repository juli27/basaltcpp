#pragma once

#include <basalt/api/gfx/device_state_cache.h>

#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/api/scene/types.h>
#include <basalt/api/math/types.h>
#include <basalt/api/base/types.h>

#include <vector>

namespace basalt::gfx {

// command list with redundant state filtering
struct CommandListRecorder final {
  void clear(const Color&);
  void draw(VertexBuffer, u32 startVertex, PrimitiveType, u32 primitiveCount);
  void set_directional_lights(const std::vector<DirectionalLight>&);
  void set_transform(TransformState, const Mat4f32&);
  void set_material(const Color& diffuse, const Color& ambient,
                    const Color& emissive);
  void set_render_state(const RenderState&);
  void set_texture(Texture);

  // TODO: stage currently not supported
  void set_texture_stage_state(u8 stage, TextureStageState, u32 value);

  void ext_draw_x_model(ext::XModel);

  [[nodiscard]] auto take_cmd_list() -> CommandList;

private:
  CommandList mCommandList;
  DeviceStateCache mDeviceState;
};

} // namespace basalt::gfx
