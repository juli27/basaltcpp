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

struct CommandLegacy;

struct CommandListRecorder final {
  void add(const CommandLegacy&);

  void set_directional_lights(const std::vector<DirectionalLight>&);
  void set_transform(TransformType, const Mat4f32&);
  void set_render_state(RenderState, u32 value);

  void ext_draw_x_model(ext::ModelHandle);

  [[nodiscard]] auto complete_command_list() -> CommandList;

private:
  CommandList mCommandList;
  DeviceStateCache mDeviceState;
};

} // namespace basalt::gfx
