#pragma once

#include "gfx/types.h"
#include "gfx/backend/types.h"
#include "gfx/backend/ext/types.h"

#include "scene/types.h"

#include "shared/types.h"

#include "math/types.h"

#include "base/types.h"

#include <entt/core/fwd.hpp>
#include <entt/entity/entity.hpp>
#include <gsl/span>

#include <functional>
#include <string>

namespace basalt {

class DebugUi final {
public:
  auto show_gfx_info(gfx::Info const&) -> void;
  auto show_performance_overlay(bool& isOpen) -> void;

  using RenderComponentUi = std::function<void(Entity const&)>;

  struct ComponentUi {
    entt::id_type typeId;
    std::string name;
    RenderComponentUi render;
  };

  struct SceneInspectorState {
    EntityId selected{entt::null};
    gsl::span<ComponentUi> componentUis{};
  };

  static auto scene_inspector(Scene&, bool& isOpen) -> void;
  static auto entities(EntityRegistry&) -> void;
  static auto entity_hierarchy_panel(EntityRegistry&,
                                     EntityId& selected) -> void;
  static auto entity_components(Entity, gsl::span<ComponentUi>) -> void;
  static auto transform(Transform&) -> void;
  static auto local_to_world(LocalToWorld const&) -> void;
  static auto camera(gfx::Camera&) -> void;
  static auto model(gfx::Model const&) -> void;
  static auto x_model(gfx::ext::XModel const&) -> void;
  static auto light(gfx::Light&) -> void;
  static auto point_light(gfx::PointLight&) -> void;
  static auto spot_light(gfx::SpotLight&) -> void;
  static auto edit_directional_light(gfx::DirectionalLight&) -> void;
  static auto edit_color3(char const* label, Color&) -> void;
  static auto edit_color4(char const* label, Color&) -> void;
  static auto display_matrix4x4(char const* label, Matrix4x4f32 const&) -> void;

private:
  u32 mSelectedAdapterIndex{0};
  u8 mOverlayCorner{2u};
};

} // namespace basalt
