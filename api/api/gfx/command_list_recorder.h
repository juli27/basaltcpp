#pragma once

#include "device_state_cache.h"

#include "backend/command_list.h"
#include "backend/types.h"

#include "backend/ext/types.h"

#include "api/scene/types.h"
#include "api/math/types.h"
#include "api/base/types.h"

#include <vector>

namespace basalt::gfx {

struct CommandListRecorder final {
  void clear(const Color&);
  void draw(VertexBuffer, PrimitiveType, u32 startVertex, u32 primitiveCount);
  void set_directional_lights(const std::vector<DirectionalLight>&);
  void set_transform(TransformState, const Mat4f32&);
  void set_material(const Color& diffuse, const Color& ambient,
                    const Color& emissive);
  void set_render_state(RenderState, u32 value);
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
