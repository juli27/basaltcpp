#include "runtime/debug.h"

#include "runtime/Scene.h"
#include "runtime/gfx/types.h"
#include "runtime/scene/transform.h"

#include "runtime/math/Constants.h"
#include "runtime/math/Vec3.h"

#include "runtime/shared/Color.h"
#include "runtime/shared/Types.h"

#include <entt/entity/registry.hpp>
#include <imgui/imgui.h>

#include <array>
#include <charconv>

using std::array;

using entt::entity;

namespace basalt {

using math::PI;
using math::Vec3f32;

namespace {

void edit_color3(const char* label, Color& color);
void edit_color4(const char* label, Color& color);

} // namespace

void Debug::draw_scene_debug_ui(bool* open, Scene* const scene) {
  if (ImGui::Begin("Scene", open, ImGuiWindowFlags_AlwaysAutoResize)) {
    edit_color3("Background Color", scene->mBackgroundColor);
    edit_color4("Ambient Light", scene->mAmbientLightColor);

    if (ImGui::CollapsingHeader("Directional Lights")) {
      ImGui::PushID("Directional Lights");
      for (uSize i = 0; i < scene->mDirectionalLights.size(); i++) {
        array<char, 16> str {};
        std::to_chars(str.data(), str.data() + str.size(), i);
        if (ImGui::TreeNode(str.data())) {
          edit_color4("Diffuse", scene->mDirectionalLights[i].diffuseColor);
          edit_color4("Ambient", scene->mDirectionalLights[i].ambientColor);

          array<f32, 3> direction = {
            scene->mDirectionalLights[i].direction.x()
          , scene->mDirectionalLights[i].direction.y()
          , scene->mDirectionalLights[i].direction.z()
          };
          ImGui::DragFloat3("Direction", direction.data(), 0.1f);

          using std::get;
          scene->mDirectionalLights[i].direction = Vec3f32::normalize(
            Vec3f32 {
              get<0>(direction), get<1>(direction), get<2>(direction)
            });

          ImGui::TreePop();
        }
      }

      ImGui::PopID();
    }

    scene->mEntityRegistry.each(
      [scene](const entity entity) -> void {
        array<char, 16> str {};
        std::to_chars(
          str.data(), str.data() + str.size(), enum_cast(entity));
        if (ImGui::CollapsingHeader(str.data())) {
          if (scene->mEntityRegistry.has<Transform>(entity)) {
            auto& transform = scene->mEntityRegistry.get<Transform>(
              entity);

            if (ImGui::TreeNodeEx(
              "Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
              f32 position[3] = {
                transform.mPosition.x(), transform.mPosition.y()
              , transform.mPosition.z()
              };
              ImGui::DragFloat3("Position", position, 0.1f);
              transform.mPosition.set(
                position[0], position[1], position[2]);

              f32 rotation[3] = {
                transform.mRotation.x(), transform.mRotation.y()
              , transform.mRotation.z()
              };
              ImGui::DragFloat3(
                "Rotation", rotation, 0.01f, 0.0f, 2.0f * PI);
              transform.mRotation.set(
                rotation[0], rotation[1], rotation[2]);

              f32 scaling[3] = {
                transform.mScale.x(), transform.mScale.y()
              , transform.mScale.z()
              };
              ImGui::DragFloat3("Scaling", scaling, 0.1f, 0.0f);
              transform.mScale.set(scaling[0], scaling[1], scaling[2]);

              ImGui::TreePop();
            }
          }

          if (scene->mEntityRegistry.has<gfx::RenderComponent>(entity)) {
            auto& renderComponent = scene->
                                    mEntityRegistry.get<
                                      gfx::RenderComponent>(entity);

            if (ImGui::TreeNodeEx(
              "RenderComponent", ImGuiTreeNodeFlags_DefaultOpen)) {
              ImGui::Text(
                "Mesh: %#x", renderComponent.mMesh.get_value());
              ImGui::Text(
                "Texture: %#x", renderComponent.mTexture.get_value());

              edit_color4("Diffuse", renderComponent.mDiffuseColor);
              edit_color4("Ambient", renderComponent.mAmbientColor);

              if (renderComponent.mRenderFlags ==
                gfx::backend::RenderFlagNone) {
                ImGui::TextUnformatted("Flag: RenderFlagNone");
              } else {
                if (renderComponent.mRenderFlags &
                  gfx::backend::RenderFlagCullNone) {
                  ImGui::TextUnformatted("Flag: RenderFlagCullNone");
                }

                if (renderComponent.mRenderFlags &
                  gfx::backend::RenderFlagDisableLighting) {
                  ImGui::TextUnformatted(
                    "Flag: RenderFlagDisableLighting");
                }
              }

              ImGui::TreePop();
            }
          }
        }
      });
  }

  ImGui::End();
}

namespace {

void edit_color3(const char* label, Color& color) {
  array<float, 3> colorArray {
    color.red(), color.green(), color.blue()
  };

  ImGui::ColorEdit3(label, colorArray.data(), ImGuiColorEditFlags_Float);

  using std::get;
  color = Color {
    get<0>(colorArray), get<1>(colorArray), get<2>(colorArray)
  };
}

void edit_color4(const char* label, Color& color) {
  array<float, 4> colorArray {
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
