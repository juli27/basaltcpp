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

  auto ecs() -> entt::registry&;

  void set_background_color(const Color& background);
  [[nodiscard]]
  auto background_color() const -> const Color&;

  void set_ambient_light(const Color&);
  [[nodiscard]]
  auto ambient_light() const -> const Color&;

  [[nodiscard]]
  auto directional_lights() const -> const std::vector<DirectionalLight>&;
  void add_directional_light(const math::Vec3f32& dir, const Color&);
  void clear_directional_lights();

private:
  friend struct Debug;

  entt::registry mEntityRegistry {};
  std::vector<DirectionalLight> mDirectionalLights {};
  Color mBackgroundColor {Colors::BLACK};
  Color mAmbientLightColor {};
};

} // namespace basalt

#endif // !BASALT_SCENE_H
