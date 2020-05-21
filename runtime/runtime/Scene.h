#pragma once
#ifndef BASALT_SCENE_H
#define BASALT_SCENE_H

#include "runtime/scene/types.h"
#include "runtime/math/Vec3.h"

#include "runtime/shared/Color.h"

#include <entt/entity/registry.hpp>

#include <vector>

namespace basalt {

struct Scene final {
  Scene() = default;

  Scene(const Scene&) = delete;
  Scene(Scene&&) = delete;

  ~Scene() = default;

  auto operator=(const Scene&) -> Scene& = delete;
  auto operator=(Scene&&) -> Scene& = delete;

  auto get_entity_registry() -> entt::registry&;

  void set_background_color(const Color& background);
  auto background_color() const -> const Color&;

  void set_ambient_light(const Color&);
  auto ambient_light() const -> const Color&;

  [[nodiscard]]
  auto directional_lights() const -> const std::vector<DirectionalLight>&;
  void add_directional_light(const math::Vec3f32& dir, const Color&);
  void clear_directional_lights();

  template <typename... Component>
  auto create_entity() {
    return mEntityRegistry.create<Component...>();
  }

  void display_debug_gui(bool* open);

private:
  entt::registry mEntityRegistry {};
  std::vector<DirectionalLight> mDirectionalLights {};
  Color mBackgroundColor {0.0f, 0.0f, 0.0f};
  Color mAmbientLightColor {};

  void display_entity_gui_impl(entt::entity entity);
};

} // namespace basalt

#endif // !BASALT_SCENE_H
