#pragma once
#ifndef BS_SCENE_H
#define BS_SCENE_H

#include <entt/entt.hpp>

#include "common/Color.h"
#include "gfx/Camera.h"
#include "math/Vec3.h"

namespace basalt {


struct TransformComponent {
  math::Vec3f32 mPosition;
  math::Vec3f32 mRotation;
  math::Vec3f32 mScale = {1.0f, 1.0f, 1.0f};
};


class Scene {
public:
  Scene();
  explicit Scene(const gfx::Camera& camera);
  Scene(Scene&&) = default;
  ~Scene() = default;

  Scene(const Scene&) = delete;

public:
  auto operator=(Scene&&) -> Scene& = default;

  auto operator=(const Scene&) -> Scene& = delete;

public:
  auto GetEntityRegistry() -> entt::registry&;

  void SetBackgroundColor(Color background);
  auto GetBackgroundColor() const -> Color;

  void SetCamera(const gfx::Camera& camera);
  auto GetCamera() const -> const gfx::Camera&;

private:
  entt::registry mEntityRegistry;
  Color mBackgroundColor;
  gfx::Camera mCamera;
};

} // namespace basalt

#endif // !BS_SCENE_H
