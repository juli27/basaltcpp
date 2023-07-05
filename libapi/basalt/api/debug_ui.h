#pragma once

#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/scene/types.h>

#include <basalt/api/shared/types.h>

#include <basalt/api/math/types.h>

#include <basalt/api/base/types.h>

namespace basalt {

class DebugUi final {
public:
  auto show_gfx_info(const gfx::Info&) -> void;
  auto show_performance_overlay(bool& isOpen) -> void;

  static auto show_scene_inspector(Scene&, bool& isOpen) -> void;
  static auto edit_scene(Scene&) -> void;
  static auto edit_ecs(EntityRegistry&) -> void;
  static auto edit_transform(Transform&) -> void;
  static auto display_local_to_world(const LocalToWorld&) -> void;
  static auto edit_directional_light(gfx::DirectionalLight&) -> void;
  static auto edit_color3(const char* label, Color&) -> void;
  static auto edit_color4(const char* label, Color&) -> void;
  static auto display_mat4(const char* label, const Matrix4x4f32&) -> void;

private:
  u32 mSelectedAdapterIndex {0};
  u8 mOverlayCorner {2u};
};

} // namespace basalt
