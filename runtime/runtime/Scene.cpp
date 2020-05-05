#include "runtime/Scene.h"

#include "runtime/math/Constants.h"

#include <imgui/imgui.h>

#include <array>
#include <charconv>

namespace basalt {

void TransformComponent::move(const f32 offsetX, const f32 offsetY
                            , const f32 offsetZ) noexcept {
  mPosition += math::Vec3f32(offsetX, offsetY, offsetZ);
}

void TransformComponent::rotate(const f32 radOffsetX, const f32 radOffsetY
                              , const f32 radOffsetZ) noexcept {
  mRotation += math::Vec3f32(radOffsetX, radOffsetY, radOffsetZ);

  constexpr auto maxAngle = math::PI * 2.0f;
  if (mRotation.x() < 0.0f) mRotation.set_x(maxAngle);
  if (mRotation.x() > maxAngle) mRotation.set_x(0.0f);
  if (mRotation.y() < 0.0f) mRotation.set_y(maxAngle);
  if (mRotation.y() > maxAngle) mRotation.set_y(0.0f);
  if (mRotation.z() < 0.0f) mRotation.set_z(maxAngle);
  if (mRotation.z() > maxAngle) mRotation.set_z(0.0f);
}

Scene::Scene(const gfx::Camera& camera) : mCamera(camera) {}

void Scene::display_entity_gui(const entt::entity entity) {
  if (ImGui::Begin("Entity")) {
    display_entity_gui_impl(entity);
  }

  ImGui::End();
}

void Scene::display_debug_gui() {
  using std::get;

  if (ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
    std::array<float, 3> clearColor {};
    get<0>(clearColor) = mBackgroundColor.red();
    get<1>(clearColor) = mBackgroundColor.green();
    get<2>(clearColor) = mBackgroundColor.blue();

    ImGui::ColorEdit3(
      "background color", clearColor.data(), ImGuiColorEditFlags_Float);

    mBackgroundColor = Color {
      get<0>(clearColor), get<1>(clearColor), get<2>(clearColor)
    };

    mEntityRegistry.each([this](const auto entity) {
      std::array<char, 16> str {};
      std::to_chars(str.data(), str.data() + str.size(), enum_cast(entity));
      if (ImGui::CollapsingHeader(str.data())) {
        display_entity_gui_impl(entity);
      }
    });
  }

  ImGui::End();
}

auto Scene::get_entity_registry() -> entt::registry& {
  return mEntityRegistry;
}

auto Scene::get_background_color() const -> const Color& {
  return mBackgroundColor;
}

void Scene::set_background_color(const Color& background) {
  mBackgroundColor = background;
}

void Scene::set_camera(const gfx::Camera& camera) {
  mCamera = camera;
}

auto Scene::get_camera() const -> const gfx::Camera& {
  return mCamera;
}

void Scene::display_entity_gui_impl(const entt::entity entity) {
  if (!mEntityRegistry.has<TransformComponent>(entity)) {
    return;
  }

  auto& transform = mEntityRegistry.get<TransformComponent>(entity);

  if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
    f32 position[3] = {transform.mPosition.x(), transform.mPosition.y()
                     , transform.mPosition.z()};
    ImGui::DragFloat3("Position", position, 0.1f);
    transform.mPosition.set(position[0], position[1], position[2]);

    f32 rotation[3] = {transform.mRotation.x(), transform.mRotation.y()
                     , transform.mRotation.z()};
    ImGui::DragFloat3("Rotation", rotation, 0.01f, 0.0f, 2.0f * math::PI);
    transform.mRotation.set(rotation[0], rotation[1], rotation[2]);

    f32 scaling[3] = {transform.mScale.x(), transform.mScale.y()
                    , transform.mScale.z()};
    ImGui::DragFloat3("Scaling", scaling, 0.1f, 0.0f);
    transform.mScale.set(scaling[0], scaling[1], scaling[2]);

    ImGui::TreePop();
  }
}

} // namespace basalt
