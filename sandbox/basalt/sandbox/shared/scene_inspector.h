#pragma once

#include <basalt/api/scene/types.h>

#include <entt/core/fwd.hpp>

#include <functional>
#include <string>
#include <vector>

class SceneInspector {
public:
  using RenderComponentUi = std::function<void(basalt::Entity const&)>;

  struct ComponentUi {
    entt::id_type typeId;
    std::string name;
    RenderComponentUi render;
  };

  SceneInspector();

  auto add_component_ui(ComponentUi) -> void;

  auto show_window(basalt::Scene&, bool& open) -> void;

private:
  basalt::EntityId mSelectedEntity;
  std::vector<ComponentUi> mComponentUis;

  auto entity_hierarchy(basalt::EntityRegistry&) -> void;

  auto entity_components(basalt::EntityRegistry&) -> void;
};
