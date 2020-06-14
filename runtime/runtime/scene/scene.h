#pragma once

#include "types.h"

#include <runtime/shared/color.h>
#include <runtime/shared/types.h>

#include <entt/entity/registry.hpp>

#include <vector>

namespace basalt {

template <typename T>
struct Vec3;
using Vec3f32 = Vec3<f32>;

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
  void add_directional_light(const Vec3f32& dir, const Color&);
  void clear_directional_lights();

private:
  friend struct Debug;

  entt::registry mEntityRegistry {};
  std::vector<DirectionalLight> mDirectionalLights {};
  Color mBackgroundColor {Colors::BLACK};
  Color mAmbientLightColor {};
};

} // namespace basalt
