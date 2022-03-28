#pragma once

#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/shared/color.h>

#include <basalt/api/math/vector3.h>

#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>

#include <vector>

namespace basalt {

struct Scene final {
  Scene() = default;

  Scene(const Scene&) = delete;
  Scene(Scene&&) = delete;

  ~Scene() noexcept = default;

  auto operator=(const Scene&) -> Scene& = delete;
  auto operator=(Scene&&) -> Scene& = delete;

  auto ecs() -> entt::registry&;

  [[nodiscard]] auto
  create_entity(const Vector3f32& position = Vector3f32 {0.0f},
                const Vector3f32& rotation = Vector3f32 {0.0f},
                const Vector3f32& scale = Vector3f32 {1.0f}) -> entt::handle;
  [[nodiscard]] auto get_handle(entt::entity) -> entt::handle;

  [[nodiscard]] auto background() const -> const Color&;
  void set_background(const Color&);

  [[nodiscard]] auto ambient_light() const -> const Color&;
  void set_ambient_light(const Color&);

  [[nodiscard]] auto directional_lights() const
    -> const std::vector<gfx::DirectionalLight>&;
  void add_directional_light(const Vector3f32& direction, const Color&);
  void clear_directional_lights();

private:
  friend struct Debug;

  entt::registry mEntityRegistry;
  std::vector<gfx::DirectionalLight> mDirectionalLights;
  Color mBackgroundColor {Colors::BLACK};
  Color mAmbientLightColor;
};

} // namespace basalt
