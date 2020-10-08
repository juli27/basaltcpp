#pragma once

#include "device_state_cache.h"

#include "backend/command_list.h"
#include "backend/types.h"

#include "backend/ext/types.h"

#include "api/shared/types.h"

#include <vector>

namespace basalt {

struct Color;
struct DirectionalLight;
struct Mat4;
using Mat4f32 = Mat4;

namespace gfx {

struct CommandLegacy;

struct CommandListRecorder final {
  void add(const CommandLegacy&);

  void set_ambient_light(const Color&);
  void set_directional_lights(const std::vector<DirectionalLight>&);
  void set_transform(TransformType, const Mat4f32&);
  void set_render_state(RenderState, u32 value);

  void ext_draw_x_model(ext::ModelHandle);

  [[nodiscard]] auto complete_command_list() -> CommandList;

private:
  CommandList mCommandList {};
  DeviceStateCache mDeviceState {};
};

} // namespace gfx
} // namespace basalt
