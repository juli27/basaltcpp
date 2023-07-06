#include <basalt/api/debug_ui.h>

#include <basalt/gfx/utils.h>

#include <basalt/api/scene/ecs.h>
#include <basalt/api/scene/scene.h>
#include <basalt/api/scene/transform.h>

#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/api/shared/color.h>

#include <basalt/api/math/constants.h>
#include <basalt/api/math/matrix4x4.h>
#include <basalt/api/math/vector3.h>

#include <basalt/api/base/enum_array.h>

#include <fmt/format.h>
#include <imgui/imgui.h>

#include <array>
#include <numeric>
#include <string>
#include <string_view>

namespace basalt {

using namespace std::literals;
using std::array;
using std::string;
using std::string_view;

namespace {

auto to_string(const gfx::BackendApi api) -> string_view {
  static constexpr EnumArray<gfx::BackendApi, string_view, 2> TO_STRING {
    {gfx::BackendApi::Default, "Default"sv},
    {gfx::BackendApi::Direct3D9, "Direct3D 9"sv},
  };
  static_assert(gfx::BACKEND_API_COUNT == TO_STRING.size());

  return TO_STRING[api];
}

auto to_string(const gfx::DisplayMode& mode,
               const gfx::ImageFormat format) noexcept -> string {
  const auto gcd {std::gcd(mode.width, mode.height)};

  return fmt::format(FMT_STRING("{}x{} ({}:{}) {}Hz {}"), mode.width,
                     mode.height, mode.width / gcd, mode.height / gcd,
                     mode.refreshRate, to_string(format));
}

} // namespace

auto DebugUi::show_gfx_info(const gfx::Info& gfxInfo) -> void {
  const gfx::AdapterInfo& selectedAdapter {
    gfxInfo.adapters[mSelectedAdapterIndex]};

  ImGui::Text("Backend API: %s", to_string(gfxInfo.backendApi).data());

  ImGui::Separator();

  if (ImGui::BeginCombo("Adapter", selectedAdapter.displayName.c_str())) {
    for (const auto& adapter : gfxInfo.adapters) {
      ImGui::PushID(&adapter);

      const u32 adapterIndex {adapter.handle.value()};
      const bool isSelected {adapterIndex == mSelectedAdapterIndex};

      if (ImGui::Selectable(adapter.displayName.c_str(), isSelected)) {
        mSelectedAdapterIndex = adapterIndex;
      }

      if (isSelected) {
        ImGui::SetItemDefaultFocus();
      }

      ImGui::PopID();
    }

    ImGui::EndCombo();
  }

  ImGui::Text("Driver: %s", selectedAdapter.driverInfo.c_str());

  ImGui::Separator();

  ImGui::TextUnformatted("Adapter Modes");

  if (ImGui::BeginChild("modes")) {
    const auto& adapterModes {selectedAdapter.adapterModes};

    for (uSize i {0}; i < adapterModes.size(); ++i) {
      const auto& adapterMode {adapterModes[i]};

      for (const auto& displayMode : adapterMode.displayModes) {
        if (adapterMode.displayFormat == selectedAdapter.displayFormat &&
            displayMode.width == selectedAdapter.displayMode.width &&
            displayMode.height == selectedAdapter.displayMode.height &&
            displayMode.refreshRate ==
              selectedAdapter.displayMode.refreshRate) {
          ImGui::PushStyleColor(ImGuiCol_Text, ImVec4 {1.0f, 1.0f, 0.0f, 1.0f});
          ImGui::TextUnformatted(
            to_string(displayMode, adapterMode.displayFormat).c_str());
          ImGui::PopStyleColor();

          if (ImGui::IsWindowAppearing()) {
            ImGui::SetScrollHereY();
          }
        } else {
          ImGui::TextUnformatted(
            to_string(displayMode, adapterMode.displayFormat).c_str());
        }
      }
    }
  }

  ImGui::EndChild();
}

auto DebugUi::show_performance_overlay(bool& isOpen) -> void {
  constexpr f32 distanceToEdge {8.0f};
  const ImGuiViewport& vp {*ImGui::GetMainViewport()};

  const f32 windowPosX {mOverlayCorner & 0x1
                          ? vp.WorkPos.x + vp.WorkSize.x - distanceToEdge
                          : vp.WorkPos.x + distanceToEdge};

  const f32 windowPosY {mOverlayCorner & 0x2
                          ? vp.WorkPos.y + vp.WorkSize.y - distanceToEdge
                          : vp.WorkPos.y + distanceToEdge};

  const ImVec2 windowPosPivot {mOverlayCorner & 0x1 ? 1.0f : 0.0f,
                               mOverlayCorner & 0x2 ? 1.0f : 0.0f};

  ImGui::SetNextWindowPos(ImVec2 {windowPosX, windowPosY}, ImGuiCond_Always,
                          windowPosPivot);
  ImGui::SetNextWindowBgAlpha(0.35f);

  constexpr ImGuiWindowFlags flags {
    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration |
    ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
    ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav};

  if (ImGui::Begin("Overlay", &isOpen, flags)) {
    const ImGuiIO& io {ImGui::GetIO()};

    const f64 fps {io.Framerate};

    ImGui::Text("%.3f ms/frame (%.1f fps)", 1000.0 / fps, fps);

    if (ImGui::BeginPopupContextWindow()) {
      if (ImGui::MenuItem("Top-left", nullptr, mOverlayCorner == 0,
                          mOverlayCorner != 0)) {
        mOverlayCorner = 0;
      }
      if (ImGui::MenuItem("Top-right", nullptr, mOverlayCorner == 1,
                          mOverlayCorner != 1)) {
        mOverlayCorner = 1;
      }
      if (ImGui::MenuItem("Bottom-left", nullptr, mOverlayCorner == 2,
                          mOverlayCorner != 2)) {
        mOverlayCorner = 2;
      }
      if (ImGui::MenuItem("Bottom-right", nullptr, mOverlayCorner == 3,
                          mOverlayCorner != 3)) {
        mOverlayCorner = 3;
      }
      if (ImGui::MenuItem("Hide")) {
        isOpen = false;
      }

      ImGui::EndPopup();
    }
  }

  ImGui::End();
}

auto DebugUi::show_scene_inspector(Scene& scene, bool& isOpen) -> void {
  ImGui::SetNextWindowSize(ImVec2 {400.0f, 600.0f}, ImGuiCond_FirstUseEver);
  if (!ImGui::Begin("Scene Inspector", &isOpen)) {
    ImGui::End();
    return;
  }

  edit_scene(scene);

  ImGui::End();
}

auto DebugUi::edit_scene(Scene& scene) -> void {
  edit_color3("Background Color", scene.mBackgroundColor);

  ImGui::Separator();

  edit_color4("Ambient Light", scene.mAmbientLightColor);

  if (!scene.mDirectionalLights.empty()) {
    ImGui::PushID("Directional Lights");
    for (uSize i {0}; i < scene.mDirectionalLights.size(); i++) {
      ImGui::PushID(static_cast<i32>(i));
      if (ImGui::TreeNode("Directional Light")) {
        edit_directional_light(scene.mDirectionalLights[i]);

        ImGui::TreePop();
      }

      ImGui::PopID();
    }

    ImGui::PopID();
  }

  ImGui::Separator();

  edit_ecs(scene.entity_registry());
}

auto DebugUi::edit_ecs(EntityRegistry& ecs) -> void {
  ecs.view<EntityId>().each([&](const EntityId entity) -> void {
    ImGui::PushID(to_integral(entity));

    if (ImGui::TreeNode("Entity", "Entity %d", to_integral(entity))) {
      if (auto* const transform {ecs.try_get<Transform>(entity)}) {
        if (ImGui::TreeNode("Transform")) {
          edit_transform(*transform);

          ImGui::TreePop();
        }
      }
      if (const auto* const localToWorld {
            ecs.try_get<const LocalToWorld>(entity)}) {
        if (ImGui::TreeNode("LocalToWorld")) {
          display_local_to_world(*localToWorld);

          ImGui::TreePop();
        }
      }

      if (const auto* const rc {ecs.try_get<gfx::RenderComponent>(entity)}) {
        if (ImGui::TreeNode("Render Component")) {
          ImGui::Text("Mesh: %#x", rc->mesh.value());
          ImGui::Text("Material: %#x", rc->material.value());

          ImGui::TreePop();
        }
      }

      if (const auto* const gfxXModel {ecs.try_get<gfx::ext::XModel>(entity)}) {
        if (ImGui::TreeNode("Gfx Model")) {
          ImGui::Text("handle = %d", gfxXModel->value());

          ImGui::TreePop();
        }
      }

      ImGui::TreePop();
    }

    ImGui::PopID();
  });
}

auto DebugUi::edit_transform(Transform& transform) -> void {
  ImGui::DragFloat3("Position", transform.position.elements.data(), 0.1f);

  ImGui::DragFloat3("Rotation", transform.rotation.elements.data(), 0.01f, -PI,
                    PI);

  ImGui::DragFloat3("Scale", transform.scale.elements.data(), 0.1f, 0.0f);
}

auto DebugUi::display_local_to_world(const LocalToWorld& localToWorld) -> void {
  display_mat4("##value", localToWorld.value);
}

auto DebugUi::edit_directional_light(gfx::DirectionalLight& light) -> void {
  edit_color4("Diffuse", light.diffuseColor);
  edit_color4("Ambient", light.ambientColor);

  ImGui::DragFloat3("Direction", light.direction.elements.data(), 0.1f);
  light.direction = Vector3f32::normalize(light.direction);
}

auto DebugUi::edit_color3(const char* label, Color& color) -> void {
  array colorArray {color.r(), color.g(), color.b()};

  ImGui::ColorEdit3(label, colorArray.data(), ImGuiColorEditFlags_Float);

  color = Color::from_non_linear(
    std::get<0>(colorArray), std::get<1>(colorArray), std::get<2>(colorArray));
}

auto DebugUi::edit_color4(const char* label, Color& color) -> void {
  array colorArray {color.r(), color.g(), color.b(), color.a()};

  ImGui::ColorEdit4(label, colorArray.data(), ImGuiColorEditFlags_Float);

  color =
    Color::from_non_linear(std::get<0>(colorArray), std::get<1>(colorArray),
                           std::get<2>(colorArray), std::get<3>(colorArray));
}

auto DebugUi::display_mat4(const char* label, const Matrix4x4f32& mat) -> void {
  const string labelString {label};

  ImGui::BeginDisabled();

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

  ImGui::EndDisabled();
}

} // namespace basalt
