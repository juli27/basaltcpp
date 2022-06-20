#pragma once

#include <basalt/api/scene/types.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/shared/types.h>

#include <basalt/api/base/types.h>

#include <entt/entity/fwd.hpp>

namespace basalt {

class DebugUi final {
public:
  auto show_gfx_info(const gfx::Info&) -> void;

  static auto show_scene_inspector(Scene&, bool& isOpen) -> void;
  static auto edit_scene(Scene&) -> void;
  static auto edit_ecs(entt::registry&) -> void;
  static auto edit_transform(Transform&) -> void;
  static auto edit_directional_light(gfx::DirectionalLight&) -> void;
  static auto edit_color3(const char* label, Color&) -> void;
  static auto edit_color4(const char* label, Color&) -> void;

private:
  u32 mSelectedAdapterIndex {0};
};

} // namespace basalt
