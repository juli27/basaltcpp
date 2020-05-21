#include "runtime/Scene.h"

#include "runtime/gfx/types.h"
#include "runtime/math/Constants.h"

#include <imgui/imgui.h>

#include <array>
#include <charconv>

using entt::entity;
using entt::registry;

namespace basalt {

using gfx::DirectionalLight;
using math::PI;
using math::Vec3f32;

namespace {

void edit_color3(const char* label, Color& color);
void edit_color4(const char* label, Color& color);

} // namespace

void TransformComponent::move(const f32 offsetX, const f32 offsetY
                            , const f32 offsetZ) noexcept {
  mPosition += Vec3f32(offsetX, offsetY, offsetZ);
}

void TransformComponent::rotate(const f32 radOffsetX, const f32 radOffsetY
                              , const f32 radOffsetZ) noexcept {
  mRotation += Vec3f32(radOffsetX, radOffsetY, radOffsetZ);

  constexpr auto maxAngle = PI * 2.0f;
  if (mRotation.x() < 0.0f) mRotation.set_x(maxAngle);
  if (mRotation.x() > maxAngle) mRotation.set_x(0.0f);
  if (mRotation.y() < 0.0f) mRotation.set_y(maxAngle);
  if (mRotation.y() > maxAngle) mRotation.set_y(0.0f);
  if (mRotation.z() < 0.0f) mRotation.set_z(maxAngle);
  if (mRotation.z() > maxAngle) mRotation.set_z(0.0f);
}

Scene::Scene(const gfx::Camera& camera) : mCamera(camera) {}

auto Scene::get_entity_registry() -> registry& {
  return mEntityRegistry;
}

auto Scene::background_color() const -> const Color& {
  return mBackgroundColor;
}

void Scene::set_background_color(const Color& background) {
  mBackgroundColor = background;
}

void Scene::set_camera(const gfx::Camera& camera) {
  mCamera = camera;
}

auto Scene::camera() const -> const gfx::Camera& {
  return mCamera;
}

void Scene::set_ambient_light(const Color& color) {
  mAmbientLightColor = color;
}

auto Scene::ambient_light() const -> const Color& {
  return mAmbientLightColor;
}

auto Scene::directional_lights() const -> const std::vector<DirectionalLight>& {
  return mDirectionalLights;
}

// TODO: ambient color support
void Scene::add_directional_light(const Vec3f32& dir, const Color& color) {
  mDirectionalLights.push_back(DirectionalLight {dir, color, Color {}});
}

void Scene::clear_directional_lights() {
  mDirectionalLights.clear();
}

void Scene::display_debug_gui() {
  if (ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
    edit_color3("Background Color", mBackgroundColor);
    edit_color4("Ambient Light", mAmbientLightColor);

    if (ImGui::CollapsingHeader("Directional Lights")) {
      ImGui::PushID("Directional Lights");
      for (uSize i = 0; i < mDirectionalLights.size(); i++) {
        std::array<char, 16> str {};
        std::to_chars(str.data(), str.data() + str.size(), i);
        if (ImGui::TreeNode(str.data())) {
          edit_color4("Diffuse", mDirectionalLights[i].diffuseColor);
          edit_color4("Ambient", mDirectionalLights[i].ambientColor);

          std::array<f32, 3> direction = {
            mDirectionalLights[i].direction.x()
          , mDirectionalLights[i].direction.y()
          , mDirectionalLights[i].direction.z()
          };
          ImGui::DragFloat3("Direction", direction.data(), 0.1f);

          using std::get;
          mDirectionalLights[i].direction = Vec3f32::normalize(
            Vec3f32 {
              get<0>(direction), get<1>(direction), get<2>(direction)
            });

          ImGui::TreePop();
        }
      }

      ImGui::PopID();
    }

    mEntityRegistry.each(
      [this](const entity entity) -> void {
        std::array<char, 16> str {};
        std::to_chars(str.data(), str.data() + str.size(), enum_cast(entity));
        if (ImGui::CollapsingHeader(str.data())) {
          this->display_entity_gui_impl(entity);
        }
      });
  }

  ImGui::End();
}

void Scene::display_entity_gui_impl(const entity entity) {
  if (mEntityRegistry.has<TransformComponent>(entity)) {
    auto& transform = mEntityRegistry.get<TransformComponent>(entity);

    if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
      f32 position[3] = {
        transform.mPosition.x(), transform.mPosition.y()
      , transform.mPosition.z()
      };
      ImGui::DragFloat3("Position", position, 0.1f);
      transform.mPosition.set(position[0], position[1], position[2]);

      f32 rotation[3] = {
        transform.mRotation.x(), transform.mRotation.y()
      , transform.mRotation.z()
      };
      ImGui::DragFloat3("Rotation", rotation, 0.01f, 0.0f, 2.0f * PI);
      transform.mRotation.set(rotation[0], rotation[1], rotation[2]);

      f32 scaling[3] = {
        transform.mScale.x(), transform.mScale.y()
      , transform.mScale.z()
      };
      ImGui::DragFloat3("Scaling", scaling, 0.1f, 0.0f);
      transform.mScale.set(scaling[0], scaling[1], scaling[2]);

      ImGui::TreePop();
    }
  }

  if (mEntityRegistry.has<gfx::RenderComponent>(entity)) {
    auto& renderComponent = mEntityRegistry.get<gfx::RenderComponent>(entity);

    if (ImGui::TreeNodeEx("RenderComponent", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::Text("Mesh: %#x", renderComponent.mMesh.get_value());
      ImGui::Text("Texture: %#x", renderComponent.mTexture.get_value());

      edit_color4("Diffuse", renderComponent.mDiffuseColor);
      edit_color4("Ambient", renderComponent.mAmbientColor);

      if (renderComponent.mRenderFlags == gfx::backend::RenderFlagNone) {
        ImGui::TextUnformatted("Flag: RenderFlagNone");
      } else {
        if (renderComponent.mRenderFlags & gfx::backend::RenderFlagCullNone) {
          ImGui::TextUnformatted("Flag: RenderFlagCullNone");
        }

        if (renderComponent.mRenderFlags & gfx::backend::RenderFlagDisableLighting) {
          ImGui::TextUnformatted("Flag: RenderFlagDisableLighting");
        }
      }

      ImGui::TreePop();
    }
  }
}

namespace {

void edit_color3(const char* label, Color& color) {
  std::array<float, 3> colorArray {
    color.red(), color.green(), color.blue()
  };

  ImGui::ColorEdit3(label, colorArray.data(), ImGuiColorEditFlags_Float);

  using std::get;
  color = Color {
    get<0>(colorArray), get<1>(colorArray), get<2>(colorArray)
  };
}

void edit_color4(const char* label, Color& color) {
  std::array<float, 4> colorArray {
    color.red(), color.green(), color.blue(), color.alpha()
  };

  ImGui::ColorEdit4(label, colorArray.data(), ImGuiColorEditFlags_Float);

  using std::get;
  color = Color {
    get<0>(colorArray), get<1>(colorArray), get<2>(colorArray)
  , get<3>(colorArray)
  };
}

} // namespace

} // namespace basalt
