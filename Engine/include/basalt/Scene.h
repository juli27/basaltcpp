#pragma once
#ifndef BS_SCENE_H
#define BS_SCENE_H

#include <entt/entt.hpp>

#include "common/Color.h"
#include "common/Types.h"
#include "gfx/Camera.h"
#include "math/Vec3.h"

namespace basalt {

struct TransformComponent {
  constexpr TransformComponent() noexcept = default;
  constexpr TransformComponent(const TransformComponent&) noexcept = default;
  constexpr TransformComponent(TransformComponent&&) noexcept = default;
  ~TransformComponent() noexcept = default;

  auto operator=(const TransformComponent&) noexcept -> TransformComponent&
    = default;
  auto operator=(TransformComponent&&) noexcept -> TransformComponent&
    = default;

  // TODO: wrap vectors in Position/Rotation/Scale class?
  void move(f32 offsetX, f32 offsetY, f32 offsetZ) noexcept;
  void rotate(f32 radOffsetX, f32 radOffsetY, f32 radOffsetZ) noexcept;

  math::Vec3f32 mPosition;
  math::Vec3f32 mRotation;
  math::Vec3f32 mScale = {1.0f, 1.0f, 1.0f};
};

struct Scene final {
  Scene() = default;
  explicit Scene(const gfx::Camera& camera);
  Scene(const Scene&) = delete;
  Scene(Scene&&) = default;
  ~Scene() = default;

  auto operator=(Scene&&) -> Scene& = default;
  auto operator=(const Scene&) -> Scene& = delete;

  void display_entity_gui(entt::entity entity);

  auto get_entity_registry() -> entt::registry&;

  void set_background_color(Color background);
  auto get_background_color() const -> Color;

  void set_camera(const gfx::Camera& camera);
  auto get_camera() const -> const gfx::Camera&;

private:
  entt::registry mEntityRegistry;
  Color mBackgroundColor = {0, 0, 0};
  gfx::Camera mCamera = {{}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}};
};

} // namespace basalt

#endif // !BS_SCENE_H
