#include "pch.h"

#include <basalt/Scene.h>

#include <entt/entt.hpp>
#include <imgui/imgui.h>

#include <basalt/math/Constants.h>
#include <basalt/math/Vec3.h>

namespace basalt {


void TransformComponent::Move(
  const f32 offsetX, const f32 offsetY, const f32 offsetZ
) noexcept {
  mPosition += math::Vec3f32(offsetX, offsetY, offsetZ);
}


void TransformComponent::Rotate(
  const f32 radOffsetX, const f32 radOffsetY, const f32 radOffsetZ
) noexcept {
  mRotation += math::Vec3f32(radOffsetX, radOffsetY, radOffsetZ);

  constexpr auto maxAngle = math::PI * 2.0f;
  if (mRotation.GetX() < 0.0f) mRotation.SetX(maxAngle);
  if (mRotation.GetX() > maxAngle) mRotation.SetX(0.0f);
  if (mRotation.GetY() < 0.0f) mRotation.SetY(maxAngle);
  if (mRotation.GetY() > maxAngle) mRotation.SetY(0.0f);
  if (mRotation.GetZ() < 0.0f) mRotation.SetZ(maxAngle);
  if (mRotation.GetZ() > maxAngle) mRotation.SetZ(0.0f);
}


Scene::Scene(const gfx::Camera& camera)
: mCamera(camera) {}


void Scene::DisplayEntityGui(const entt::entity entity) {
  auto& transform = mEntityRegistry.get<TransformComponent>(entity);
  if (ImGui::Begin("Entity")) {
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
      f32 position[3] = {transform.mPosition.GetX(), transform.mPosition.GetY(), transform.mPosition.GetZ()};
      ImGui::DragFloat3("Position", position, 0.1f);
      transform.mPosition.Set(position[0], position[1], position[2]);

      f32 rotation[3] = {transform.mRotation.GetX(), transform.mRotation.GetY(), transform.mRotation.GetZ()};
      ImGui::DragFloat3("Rotation", rotation, 0.01f, 0.0f, 2.0f * math::PI);
      transform.mRotation.Set(rotation[0], rotation[1], rotation[2]);

      f32 scaling[3] = {transform.mScale.GetX(), transform.mScale.GetY(), transform.mScale.GetZ()};
      ImGui::DragFloat3("Scaling", scaling, 0.1f, 0.0f);
      transform.mScale.Set(scaling[0], scaling[1], scaling[2]);
    }
  }

  ImGui::End();
}


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
