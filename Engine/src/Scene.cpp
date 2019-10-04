#include <basalt/Scene.h>

#include <basalt/math/Constants.h>
#include <basalt/math/Vec3.h>

#include <entt/entt.hpp>
#include <imgui/imgui.h>

namespace basalt {

void TransformComponent::move(
  const f32 offsetX, const f32 offsetY, const f32 offsetZ
) noexcept {
  mPosition += math::Vec3f32(offsetX, offsetY, offsetZ);
}

void TransformComponent::rotate(
  const f32 radOffsetX, const f32 radOffsetY, const f32 radOffsetZ
) noexcept {
  mRotation += math::Vec3f32(radOffsetX, radOffsetY, radOffsetZ);

  constexpr auto maxAngle = math::PI * 2.0f;
  if (mRotation.get_x() < 0.0f) mRotation.set_x(maxAngle);
  if (mRotation.get_x() > maxAngle) mRotation.set_x(0.0f);
  if (mRotation.get_y() < 0.0f) mRotation.set_y(maxAngle);
  if (mRotation.get_y() > maxAngle) mRotation.set_y(0.0f);
  if (mRotation.get_z() < 0.0f) mRotation.set_z(maxAngle);
  if (mRotation.get_z() > maxAngle) mRotation.set_z(0.0f);
}

Scene::Scene(const gfx::Camera& camera)
: mCamera(camera) {}

void Scene::display_entity_gui(const entt::entity entity) {
  auto& transform = mEntityRegistry.get<TransformComponent>(entity);
  if (ImGui::Begin("Entity")) {
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
      f32 position[3] = {transform.mPosition.get_x(), transform.mPosition.get_y(), transform.mPosition.get_z()};
      ImGui::DragFloat3("Position", position, 0.1f);
      transform.mPosition.set(position[0], position[1], position[2]);

      f32 rotation[3] = {transform.mRotation.get_x(), transform.mRotation.get_y(), transform.mRotation.get_z()};
      ImGui::DragFloat3("Rotation", rotation, 0.01f, 0.0f, 2.0f * math::PI);
      transform.mRotation.set(rotation[0], rotation[1], rotation[2]);

      f32 scaling[3] = {transform.mScale.get_x(), transform.mScale.get_y(), transform.mScale.get_z()};
      ImGui::DragFloat3("Scaling", scaling, 0.1f, 0.0f);
      transform.mScale.set(scaling[0], scaling[1], scaling[2]);
    }
  }

  ImGui::End();
}

auto Scene::get_entity_registry() -> entt::registry& {
  return mEntityRegistry;
}

auto Scene::get_background_color() const -> Color {
  return mBackgroundColor;
}

void Scene::set_background_color(Color background) {
  mBackgroundColor = background;
}

void Scene::set_camera(const gfx::Camera& camera) {
  mCamera = camera;
}

auto Scene::get_camera() const -> const gfx::Camera& {
  return mCamera;
}

} // namespace basalt
