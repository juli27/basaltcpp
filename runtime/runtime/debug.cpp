#include "runtime/debug.h"

#include "runtime/gfx/types.h"

#include "runtime/scene/scene.h"
#include "runtime/scene/transform.h"

#include "runtime/math/Constants.h"
#include "runtime/math/Vec3.h"

#include "runtime/shared/Color.h"
#include "runtime/shared/Types.h"

#include <entt/entity/registry.hpp>
#include <imgui/imgui.h>

#include <array>

using std::array;

using entt::entity;

namespace basalt {

using math::PI;
using math::Vec3f32;

namespace {

void edit_color3(const char* label, Color& color);
void edit_color4(const char* label, Color& color);

} // namespace

void Debug::draw_scene_debug_ui(bool* const open, Scene* const scene) {
  ImGui::SetNextWindowSize(ImVec2 {400.0f, 600.0f}, ImGuiCond_FirstUseEver);
  if (!ImGui::Begin("Scene Inspector", open)) {
    ImGui::End();
    return;
  }

  edit_color3("Background Color", scene->mBackgroundColor);

  ImGui::Separator();

  edit_color4("Ambient Light", scene->mAmbientLightColor);

  if (!scene->mDirectionalLights.empty()) {
    ImGui::PushID("Directional Lights");
    for (uSize i = 0; i < scene->mDirectionalLights.size(); i++) {
      ImGui::PushID(static_cast<i32>(i));
      if (ImGui::TreeNode(
        "Directional Light", "Directional Light %zo", i + 1)) {
        edit_color4("Diffuse", scene->mDirectionalLights[i].diffuseColor);
        edit_color4("Ambient", scene->mDirectionalLights[i].ambientColor);

        array<f32, 3> direction = {
          scene->mDirectionalLights[i].direction.x
        , scene->mDirectionalLights[i].direction.y
        , scene->mDirectionalLights[i].direction.z
        };
        ImGui::DragFloat3("Direction", direction.data(), 0.1f);

        using std::get;
        scene->mDirectionalLights[i].direction = Vec3f32::normalize(
          Vec3f32 {
            get<0>(direction), get<1>(direction), get<2>(direction)
          });

        ImGui::TreePop();
      }

      ImGui::PopID();
    }

    ImGui::PopID();
  }

  ImGui::Separator();

  scene->mEntityRegistry.each(
    [scene](const entity entity) -> void {
      ImGui::PushID(enum_cast(entity));

      if (ImGui::TreeNode("Entity", "Entity %d", enum_cast(entity))) {
        // show its transform component
        if (auto* const transform = scene->
                                    mEntityRegistry.try_get<Transform>(
                                      entity)) {
          if (ImGui::TreeNode("Transform")) {
            ImGui::DragFloat3("Position", &transform->mPosition.x, 0.1f);

            ImGui::DragFloat3(
              "Rotation", &transform->mRotation.x, 0.01f, 0.0f
            , 2.0f * PI);

            ImGui::DragFloat3("Scale", &transform->mScale.x, 0.1f, 0.0f);

            ImGui::TreePop();
          }
        }

        if (auto* const rc = scene->
                             mEntityRegistry.try_get<gfx::RenderComponent
                             >(
                               entity)) {
          if (ImGui::TreeNode("RenderComponent")) {
            if (rc->model) {
              ImGui::Text("Model: %#x", rc->model.get_value());
            } else {
              ImGui::Text(
                "Mesh: %#x", rc->mMesh.get_value());
              ImGui::Text(
                "Texture: %#x", rc->mTexture.get_value());

              edit_color4("Diffuse", rc->mDiffuseColor);
              edit_color4("Ambient", rc->mAmbientColor);
            }

            if (rc->mRenderFlags ==
              gfx::backend::RenderFlagNone) {
              ImGui::TextUnformatted("Flag: RenderFlagNone");
            } else {
              if (rc->mRenderFlags &
                gfx::backend::RenderFlagCullNone) {
                ImGui::TextUnformatted("Flag: RenderFlagCullNone");
              }

              if (rc->mRenderFlags &
                gfx::backend::RenderFlagDisableLighting) {
                ImGui::TextUnformatted(
                  "Flag: RenderFlagDisableLighting");
              }
            }

            ImGui::TreePop();
          }
        }

        ImGui::TreePop();
      }

      ImGui::PopID();
    });

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
