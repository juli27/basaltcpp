#include "pch.h"

#include <basalt/Scene.h>

namespace basalt {


Scene::Scene()
: Scene(gfx::Camera({}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f})) {}


Scene::Scene(const gfx::Camera& camera)
: mBackgroundColor(0,0,0)
, mCamera(camera) {}


auto Scene::GetEntityRegistry() -> entt::registry& {
  return mEntityRegistry;
}


auto Scene::GetBackgroundColor() const -> Color {
  return mBackgroundColor;
}


void Scene::SetBackgroundColor(Color background) {
  mBackgroundColor = background;
}


void Scene::SetCamera(const gfx::Camera& camera) {
  mCamera = camera;
}


auto Scene::GetCamera() const -> const gfx::Camera& {
  return mCamera;
}

} // namespace basalt
