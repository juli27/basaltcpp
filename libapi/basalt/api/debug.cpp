#include "debug.h"

#include "gfx/types.h"
#include "gfx/backend/ext/types.h"

#include "scene/scene.h"
#include "scene/transform.h"

#include "math/constants.h"
#include "math/vector3.h"

#include "shared/color.h"

#include "base/types.h"

#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>

#include <imgui/imgui.h>

#include <array>

using std::array;

using entt::entity;

namespace basalt {

namespace {

bool sShowSceneDebugUi {false};
bool sShowDemo {false};
bool sShowMetrics {false};
bool sShowAbout {false};

void edit_color3(const char* label, Color& color) {
  array<f32, 3> colorArray {color.red(), color.green(), color.blue()};

  ImGui::ColorEdit3(label, colorArray.data(), ImGuiColorEditFlags_Float);

  using std::get;
  color = Color {get<0>(colorArray), get<1>(colorArray), get<2>(colorArray)};
}

void edit_color4(const char* label, Color& color) {
  array<f32, 4> colorArray {color.red(), color.green(), color.blue(),
                            color.alpha()};

  ImGui::ColorEdit4(label, colorArray.data(), ImGuiColorEditFlags_Float);

  using std::get;
  color = Color {get<0>(colorArray), get<1>(colorArray), get<2>(colorArray),
                 get<3>(colorArray)};
}

} // namespace

void Debug::update() {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("View")) {
      ImGui::MenuItem("Scene Debug UI", nullptr, &sShowSceneDebugUi);
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Help")) {
      ImGui::MenuItem("Dear ImGui Demo", nullptr, &sShowDemo);
      ImGui::MenuItem("Dear ImGui Metrics", nullptr, &sShowMetrics);
      ImGui::MenuItem("About Dear ImGui", nullptr, &sShowAbout);

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }

  if (sShowDemo) {
    ImGui::ShowDemoWindow(&sShowDemo);
  }
  if (sShowMetrics) {
    ImGui::ShowMetricsWindow(&sShowMetrics);
  }
  if (sShowAbout) {
    ImGui::ShowAboutWindow(&sShowAbout);
  }
}

void Debug::update(Scene& scene) {
  if (sShowSceneDebugUi) {
    draw_scene_debug_ui(scene);
  }
}

void Debug::draw_scene_debug_ui(Scene& scene) {
  ImGui::SetNextWindowSize(ImVec2 {400.0f, 600.0f}, ImGuiCond_FirstUseEver);
  if (!ImGui::Begin("Scene Inspector", &sShowSceneDebugUi)) {
    ImGui::End();
    return;
  }

  edit_color3("Background Color", scene.mBackgroundColor);

  ImGui::Separator();

  edit_color4("Ambient Light", scene.mAmbientLightColor);

  if (!scene.mDirectionalLights.empty()) {
    ImGui::PushID("Directional Lights");
    for (uSize i = 0; i < scene.mDirectionalLights.size(); i++) {
      ImGui::PushID(static_cast<i32>(i));
      if (ImGui::TreeNode("Directional Light")) {
        edit_color4("Diffuse", scene.mDirectionalLights[i].diffuseColor);
        edit_color4("Ambient", scene.mDirectionalLights[i].ambientColor);

        ImGui::DragFloat3("Direction",
                          scene.mDirectionalLights[i].direction.elements.data(),
                          0.1f);
        scene.mDirectionalLights[i].direction =
          Vector3f32::normalize(scene.mDirectionalLights[i].direction);

        ImGui::TreePop();
      }

      ImGui::PopID();
    }

    ImGui::PopID();
  }

  ImGui::Separator();

  scene.mEntityRegistry.each([&scene](const entity entity) -> void {
    ImGui::PushID(to_integral(entity));

    if (ImGui::TreeNode("Entity", "Entity %d", to_integral(entity))) {
      // show its transform component
      if (auto* const transform =
            scene.mEntityRegistry.try_get<Transform>(entity)) {
        if (ImGui::TreeNode("Transform")) {
          ImGui::DragFloat3("Position", transform->position.elements.data(),
                            0.1f);

          ImGui::DragFloat3("Rotation", transform->rotation.elements.data(),
                            0.01f, -PI, PI);

          ImGui::DragFloat3("Scale", transform->scale.elements.data(), 0.1f,
                            0.0f);

          ImGui::TreePop();
        }
      }

      if (auto* const rc =
            scene.mEntityRegistry.try_get<gfx::RenderComponent>(entity)) {
        if (ImGui::TreeNode("RenderComponent")) {
          ImGui::Text("Mesh: %#x", rc->mesh.value());
          ImGui::Text("Material: %#x", rc->material.value());

          ImGui::TreePop();
        }
      }

      if (auto* const gfxXModel =
            scene.mEntityRegistry.try_get<gfx::ext::XModel>(entity)) {
        if (ImGui::TreeNode("Gfx Model")) {
          ImGui::Text("handle = %d", gfxXModel->value());

          ImGui::TreePop();
        }
      }

      ImGui::TreePop();
    }

    ImGui::PopID();
  });

  ImGui::End();
}

} // namespace basalt
