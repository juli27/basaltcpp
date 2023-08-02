#pragma once

#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>
#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/api/scene/types.h>

#include <basalt/api/shared/types.h>

#include <basalt/api/math/types.h>

#include <basalt/api/base/types.h>

#include <entt/core/fwd.hpp>
#include <entt/entity/entity.hpp>
#include <gsl/span>

#include <functional>
#include <string>

namespace basalt {

class DebugUi final {
public:
  auto show_gfx_info(const gfx::Info&) -> void;
  auto show_performance_overlay(bool& isOpen) -> void;

  using RenderComponentUi = std::function<void(const Entity&)>;

  struct ComponentUi {
    entt::id_type typeId;
    std::string name;
    RenderComponentUi render;
  };

  struct SceneInspectorState {
    EntityId selected {entt::null};
    gsl::span<ComponentUi> componentUis {};
  };

  static auto scene_inspector(Scene&, bool& isOpen) -> void;
  static auto entities(EntityRegistry&) -> void;
  static auto entity_hierarchy_panel(EntityRegistry&, EntityId& selected)
    -> void;
  static auto entity_components(Entity, gsl::span<ComponentUi>) -> void;
  static auto transform(Transform&) -> void;
  static auto local_to_world(const LocalToWorld&) -> void;
  static auto camera(gfx::Camera&) -> void;
  static auto render_component(const gfx::RenderComponent&) -> void;
  static auto x_model(const gfx::ext::XModel&) -> void;
  static auto point_light(gfx::PointLightComponent&) -> void;
  static auto edit_directional_light(gfx::DirectionalLight&) -> void;
  static auto edit_color3(const char* label, Color&) -> void;
  static auto edit_color4(const char* label, Color&) -> void;
  static auto display_matrix4x4(const char* label, const Matrix4x4f32&) -> void;

private:
  u32 mSelectedAdapterIndex {0};
  u8 mOverlayCorner {2u};
};

} // namespace basalt
