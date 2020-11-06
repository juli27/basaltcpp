#include "debug.h"

#include "gfx/types.h"
#include "gfx/backend/command_list.h"
#include "gfx/backend/commands.h"
#include "gfx/backend/types.h"
#include "gfx/backend/ext/dear_imgui_renderer.h"
#include "gfx/backend/ext/x_model_support.h"

#include "scene/scene.h"
#include "scene/transform.h"
#include "scene/types.h"

#include "math/constants.h"
#include "math/vector3.h"

#include "shared/color.h"

#include "base/types.h"
#include "base/utils.h"

#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>

#include <imgui/imgui.h>

#include <array>
#include <string>

using std::array;
using std::string;
using namespace std::literals;

using entt::entity;

namespace basalt {

using gfx::CommandClear;
using gfx::CommandDraw;
using gfx::CommandSetDirectionalLights;
using gfx::CommandSetMaterial;
using gfx::CommandSetRenderState;
using gfx::CommandSetTexture;
using gfx::CommandSetTextureStageState;
using gfx::CommandSetTransform;
using gfx::PrimitiveType;
using gfx::RenderState;
using gfx::TextureStageState;

using gfx::ext::CommandDrawXModel;
using gfx::ext::CommandRenderDearImGui;

namespace {

bool sShowSceneDebugUi {false};
bool sShowCompositeDebugUi {false};
bool sShowDemo {false};
bool sShowMetrics {false};
bool sShowAbout {false};

void show_overlay() {
  static i8 corner = 2;

  const f32 distanceToEdge = 10.0f;
  auto& io = ImGui::GetIO();
  const f32 windowPosX =
    corner & 0x1 ? io.DisplaySize.x - distanceToEdge : distanceToEdge;
  const f32 windowPosY =
    corner & 0x2 ? io.DisplaySize.y - distanceToEdge : distanceToEdge + 15.0f;
  const ImVec2 windowPosPivot {corner & 0x1 ? 1.0f : 0.0f,
                               corner & 0x2 ? 1.0f : 0.0f};

  ImGui::SetNextWindowPos(ImVec2 {windowPosX, windowPosY}, ImGuiCond_Always,
                          windowPosPivot);
  ImGui::SetNextWindowBgAlpha(0.35f);

  const ImGuiWindowFlags flags =
    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration |
    ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
    ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

  if (ImGui::Begin("Overlay", nullptr, flags)) {
    ImGui::Text("%.3f ms/frame (%.1f fps)", 1000.0f / io.Framerate,
                io.Framerate);

    if (ImGui::BeginPopupContextWindow()) {
      if (ImGui::MenuItem("Top-left", nullptr, corner == 0, corner != 0)) {
        corner = 0;
      }
      if (ImGui::MenuItem("Top-right", nullptr, corner == 1, corner != 1)) {
        corner = 1;
      }
      if (ImGui::MenuItem("Bottom-left", nullptr, corner == 2, corner != 2)) {
        corner = 2;
      }
      if (ImGui::MenuItem("Bottom-right", nullptr, corner == 3, corner != 3)) {
        corner = 3;
      }

      ImGui::EndPopup();
    }
  }

  ImGui::End();
}

void edit_color3(const char* label, Color& color) {
  array<float, 3> colorArray {color.red(), color.green(), color.blue()};

  ImGui::ColorEdit3(label, colorArray.data(), ImGuiColorEditFlags_Float);

  using std::get;
  color = Color {get<0>(colorArray), get<1>(colorArray), get<2>(colorArray)};
}

void edit_color4(const char* label, Color& color) {
  array<float, 4> colorArray {color.red(), color.green(), color.blue(),
                              color.alpha()};

  ImGui::ColorEdit4(label, colorArray.data(), ImGuiColorEditFlags_Float);

  using std::get;
  color = Color {get<0>(colorArray), get<1>(colorArray), get<2>(colorArray),
                 get<3>(colorArray)};
}

void display_color4(const char* label, const Color& color) {
  array<float, 4> colorArray {color.red(), color.green(), color.blue(),
                              color.alpha()};

  ImGui::ColorEdit4(label, colorArray.data(),
                    ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoPicker |
                      ImGuiColorEditFlags_NoInputs |
                      ImGuiColorEditFlags_NoDragDrop);
}

void display_vec3(const char* label, const Vector3f32& vec) {
  array<f32, 3> vecArr {vec.x(), vec.y(), vec.z()};

  ImGui::InputFloat3(label, vecArr.data(), "%.3f",
                     ImGuiInputTextFlags_ReadOnly);
}

void display_mat4(const char* label, const Mat4f32& mat) {
  const string labelString {label};

  array<f32, 4> firstRow {mat.m11, mat.m12, mat.m13, mat.m14};
  ImGui::InputFloat4((labelString + " Row 1"s).c_str(), firstRow.data(), "%.3f",
                     ImGuiInputTextFlags_ReadOnly);

  array<f32, 4> secondRow {mat.m21, mat.m22, mat.m23, mat.m24};
  ImGui::InputFloat4((labelString + " Row 2"s).c_str(), secondRow.data(),
                     "%.3f", ImGuiInputTextFlags_ReadOnly);

  array<f32, 4> thirdRow {mat.m31, mat.m32, mat.m33, mat.m34};
  ImGui::InputFloat4((labelString + " Row 3"s).c_str(), thirdRow.data(), "%.3f",
                     ImGuiInputTextFlags_ReadOnly);

  array<f32, 4> forthRow {mat.m41, mat.m42, mat.m43, mat.m44};
  ImGui::InputFloat4((labelString + " Row 4"s).c_str(), forthRow.data(), "%.3f",
                     ImGuiInputTextFlags_ReadOnly);
}

#define ENUM_TO_STRING(e)                                                      \
  case e:                                                                      \
    return #e

auto to_string(const PrimitiveType primitiveType) -> const char* {
  switch (primitiveType) {
    ENUM_TO_STRING(PrimitiveType::PointList);
    ENUM_TO_STRING(PrimitiveType::LineList);
    ENUM_TO_STRING(PrimitiveType::LineStrip);
    ENUM_TO_STRING(PrimitiveType::TriangleList);
    ENUM_TO_STRING(PrimitiveType::TriangleStrip);
    ENUM_TO_STRING(PrimitiveType::TriangleFan);
  }

  return "(unknown)";
}

#undef ENUM_TO_STRING

auto to_string(const RenderState state) -> const char* {
  switch (state) {
  case RenderState::Lighting:
    return "RenderState::Lighting";

  case RenderState::Ambient:
    return "RenderState::Ambient";

  case RenderState::CullMode:
    return "RenderState::CullMode";
  }

  return "(unknown)";
}

auto to_string(const TextureStageState state) -> const char* {
  constexpr array<const char*, 2> strings {
    "TextureStageState::CoordinateSource",
    "TextureStageState::TextureTransformFlags",
  };
  static_assert(gfx::TEXTURE_STAGE_STATE_COUNT == strings.size());

  return strings[enum_cast(state)];
}

// auto to_string(const gfx::TexCoordinateSrc texCoordinateSrc) -> const char* {
//  switch (texCoordinateSrc) {
//  case gfx::TcsVertex:
//    return "TcsVertex";
//
//  case gfx::TcsVertexPositionCameraSpace:
//    return "TcsVertexPositionCameraSpace";
//  }
//
//  return "(unknown)";
//}

auto to_string(const gfx::TransformState transformType) -> const char* {
  switch (transformType) {
  case gfx::TransformState::Projection:
    return "TransformState::Projection";

  case gfx::TransformState::View:
    return "TransformState::View";

  case gfx::TransformState::World:
    return "TransformState::World";

  case gfx::TransformState::Texture:
    return "TransformState::Texture";
  }

  return "(unknown)";
}

void display(const CommandClear& cmd) {
  ImGui::TextUnformatted("Clear");
  if (ImGui::IsItemHovered()) {
    ImGui::BeginTooltip();

    display_color4("color", cmd.color);

    ImGui::EndTooltip();
  }
}

void display(const CommandDraw& cmd) {
  ImGui::TextUnformatted("Draw");
  if (ImGui::IsItemHovered()) {
    ImGui::BeginTooltip();

    ImGui::Text("vertexBuffer = %#x", cmd.vertexBuffer.value());
    ImGui::Text("startVertex = %u", cmd.startVertex);
    ImGui::Text("primitiveType = %s", to_string(cmd.primitiveType));
    ImGui::Text("primitiveCount = %u", cmd.primitiveCount);

    ImGui::EndTooltip();
  }
}

void display(const CommandSetDirectionalLights& command) {
  ImGui::TextUnformatted("SetDirectionalLights");
  if (ImGui::IsItemHovered()) {
    ImGui::BeginTooltip();

    i32 i = 0;
    for (const auto& light : command.directionalLights) {
      ImGui::PushID(i++);

      display_vec3("direction", light.direction);
      display_color4("diffuseColor", light.diffuseColor);
      display_color4("ambientColor", light.ambientColor);

      ImGui::PopID();
    }

    ImGui::EndTooltip();
  }
}

void display(const CommandSetTransform& command) {
  ImGui::TextUnformatted("SetTransform");
  if (ImGui::IsItemHovered()) {
    ImGui::BeginTooltip();

    ImGui::Text("state = %s", to_string(command.state));

    display_mat4("##transform", command.transform);

    ImGui::EndTooltip();
  }
}

void display(const CommandSetMaterial& command) {
  ImGui::TextUnformatted("SetMaterial");
  if (ImGui::IsItemHovered()) {
    ImGui::BeginTooltip();

    display_color4("diffuseColor", command.diffuse);
    display_color4("ambientColor", command.ambient);
    display_color4("emissiveColor", command.emissive);

    ImGui::EndTooltip();
  }
}

void display(const CommandSetRenderState& command) {
  ImGui::TextUnformatted("SetRenderState");
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("renderState = %s\nvalue = %#x",
                      to_string(command.renderState), command.value);
  }
}

void display(const CommandSetTexture& command) {
  ImGui::TextUnformatted("SetTexture");
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("handle = %#x", command.texture);
  }
}

void display(const CommandSetTextureStageState& command) {
  ImGui::TextUnformatted("SetTextureStageState");
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("state = %s\nvalue = %#x", to_string(command.state),
                      command.value);
  }
}

void display(const CommandDrawXModel& cmd) {
  ImGui::TextUnformatted("ExtDrawXModel");
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("model = %#x", cmd.handle.value());
  }
}

void display(const CommandRenderDearImGui&) {
  ImGui::TextUnformatted("ExtRenderDearImGui");
}

} // namespace

void Debug::update() {
  show_overlay();

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("View")) {
      ImGui::MenuItem("Scene Debug UI", nullptr, &sShowSceneDebugUi);
      ImGui::MenuItem("Composite Inspector", nullptr, &sShowCompositeDebugUi);
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

#define DISPLAY(commandStruct)                                                 \
  case commandStruct::TYPE:                                                    \
    display(command->as<commandStruct>());                                     \
    break

void Debug::update(const gfx::Composite& composite) {
  if (!sShowCompositeDebugUi) {
    return;
  }

  if (!ImGui::Begin("Composite Inspector", &sShowCompositeDebugUi)) {
    ImGui::End();
    return;
  }

  i32 i = 0;
  for (const auto& cmdList : composite) {
    ImGui::PushID(i);
    if (ImGui::TreeNode("Part", "Command List (%llu commands)",
                        cmdList.commands().size())) {
      for (const auto& command : cmdList.commands()) {
        switch (command->type) {
          DISPLAY(CommandClear);
          DISPLAY(CommandDraw);
          DISPLAY(CommandSetDirectionalLights);
          DISPLAY(CommandSetTransform);
          DISPLAY(CommandSetMaterial);
          DISPLAY(CommandSetRenderState);
          DISPLAY(CommandSetTexture);
          DISPLAY(CommandSetTextureStageState);

          DISPLAY(CommandDrawXModel);
          DISPLAY(CommandRenderDearImGui);
        }
      }

      ImGui::TreePop();
    }

    ImGui::PopID();
    ++i;
  }

  ImGui::End();
}

#undef DISPLAY

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
          ImGui::Text("Texture: %#x", rc->texture.value());
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
