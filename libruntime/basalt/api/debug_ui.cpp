#include <basalt/api/debug_ui.h>

#include <basalt/gfx/utils.h>

#include <basalt/api/scene/ecs.h>
#include <basalt/api/scene/scene.h>
#include <basalt/api/scene/transform.h>

#include <basalt/api/gfx/camera.h>

#include <basalt/api/shared/color.h>
#include <basalt/api/shared/config.h>

#include <basalt/api/math/angle.h>
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
#include <variant>

namespace basalt {

using namespace std::literals;
using std::array;
using std::string;
using std::string_view;

namespace {

auto to_string(gfx::BackendApi const api) -> string_view {
  static constexpr auto TO_STRING = EnumArray<gfx::BackendApi, string_view, 2>{
    {gfx::BackendApi::Default, "Default"sv},
    {gfx::BackendApi::Direct3D9, "Direct3D 9"sv},
  };
  static_assert(gfx::BACKEND_API_COUNT == TO_STRING.size());

  return TO_STRING[api];
}

auto to_string(gfx::DisplayMode const& mode,
               gfx::ImageFormat const format) noexcept -> string {
  auto const gcd = std::gcd(mode.width, mode.height);

  return fmt::format(FMT_STRING("{}x{} ({}:{}) {}Hz {}"), mode.width,
                     mode.height, mode.width / gcd, mode.height / gcd,
                     mode.refreshRate, to_string(format));
}

} // namespace

auto DebugUi::show_gfx_info(gfx::Info const& gfxInfo) -> void {
  auto const& selectedAdapter = gfxInfo.adapters[mSelectedAdapterIndex];

  ImGui::Text("Backend API: %s", to_string(gfxInfo.backendApi).data());

  ImGui::Separator();

  if (ImGui::BeginCombo("Adapter", selectedAdapter.displayName.c_str())) {
    for (auto const& adapter : gfxInfo.adapters) {
      ImGui::PushID(&adapter);

      auto const adapterIndex = adapter.handle.value();
      auto const isSelected = adapterIndex == mSelectedAdapterIndex;

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
    auto const& adapterModes = selectedAdapter.adapterModes;

    for (auto i = uSize{0}; i < adapterModes.size(); ++i) {
      auto const& adapterMode = adapterModes[i];

      for (auto const& displayMode : adapterMode.displayModes) {
        if (adapterMode.displayFormat == selectedAdapter.displayFormat &&
            displayMode.width == selectedAdapter.displayMode.width &&
            displayMode.height == selectedAdapter.displayMode.height &&
            displayMode.refreshRate ==
              selectedAdapter.displayMode.refreshRate) {
          ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{1.0f, 1.0f, 0.0f, 1.0f});
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
  constexpr auto distanceToEdge = 8.0f;
  auto const& vp = *ImGui::GetMainViewport();

  auto const windowPosX = mOverlayCorner & 0x1
                            ? vp.WorkPos.x + vp.WorkSize.x - distanceToEdge
                            : vp.WorkPos.x + distanceToEdge;

  auto const windowPosY = mOverlayCorner & 0x2
                            ? vp.WorkPos.y + vp.WorkSize.y - distanceToEdge
                            : vp.WorkPos.y + distanceToEdge;

  auto const windowPosPivot = ImVec2{mOverlayCorner & 0x1 ? 1.0f : 0.0f,
                                     mOverlayCorner & 0x2 ? 1.0f : 0.0f};

  ImGui::SetNextWindowPos(ImVec2{windowPosX, windowPosY}, ImGuiCond_Always,
                          windowPosPivot);
  ImGui::SetNextWindowBgAlpha(0.35f);

  constexpr auto flags =
    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration |
    ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
    ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

  if (ImGui::Begin("Overlay", &isOpen, flags)) {
    auto const& io = ImGui::GetIO();

    auto const fps = f64{io.Framerate};

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

// TODO: turn scene inspector into a system
// TODO: show systems and allow to enable/disable them
// TODO: how to display system state like main camera and gfx resource cache
auto DebugUi::scene_inspector(Scene& scene, bool& isOpen) -> void {
  ImGui::SetNextWindowSize(ImVec2{400.0f, 600.0f}, ImGuiCond_FirstUseEver);
  if (!ImGui::Begin("Scene Inspector", &isOpen)) {
    ImGui::End();
    return;
  }

  entities(scene.entity_registry());

  ImGui::End();
}

auto DebugUi::entities(EntityRegistry& entities) -> void {
  auto& state = entities.ctx().get<SceneInspectorState>();

  constexpr auto childFlags = ImGuiWindowFlags_HorizontalScrollbar;

  if (ImGui::BeginChild("hierarchy",
                        {ImGui::GetContentRegionAvail().x * 0.25f, 0}, false,
                        childFlags)) {
    entity_hierarchy_panel(entities, state.selected);
  }
  ImGui::EndChild();

  ImGui::SameLine();

  if (ImGui::BeginChild("components", {}, 0, childFlags)) {
    entity_components(Entity{entities, state.selected}, state.componentUis);
  }
  ImGui::EndChild();
}

namespace {

auto entity_node(Entity const entity, EntityId& selected) -> void {
  constexpr auto baseNodeFlags = ImGuiTreeNodeFlags_OpenOnDoubleClick |
                                 ImGuiTreeNodeFlags_OpenOnArrow |
                                 ImGuiTreeNodeFlags_SpanAvailWidth;
  constexpr auto leafNodeFlags = baseNodeFlags | ImGuiTreeNodeFlags_Leaf;
  ImGui::PushID(to_integral(entity.entity()));

  auto const* children = entity.try_get<Children>();
  auto const isParent = children != nullptr;
  auto nodeFlags = isParent ? baseNodeFlags : leafNodeFlags;

  if (selected == entity) {
    nodeFlags |= ImGuiTreeNodeFlags_Selected;
  }

  auto const entityNode = [](Entity const e,
                             ImGuiTreeNodeFlags const flags) -> bool {
    if (auto const* name{e.try_get<EntityName const>()}) {
      return ImGui::TreeNodeEx(name->value.c_str(), flags);
    }

    return ImGui::TreeNodeEx("Entity", flags, "Entity %d",
                             to_integral(e.entity()));
  };

  auto const open = entityNode(entity, nodeFlags);

  if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
    selected = entity;
  }

  if (open) {
    if (children) {
      for (auto const childId : children->ids) {
        entity_node(Entity{*entity.registry(), childId}, selected);
      }
    }

    ImGui::TreePop();
  }

  ImGui::PopID();
}

} // namespace

auto DebugUi::entity_hierarchy_panel(EntityRegistry& entities,
                                     EntityId& selected) -> void {
  auto const rootEntities = entities.view<EntityId>(entt::exclude<Parent>);

  for (auto const id : rootEntities) {
    entity_node(Entity{entities, id}, selected);
  }
}

auto DebugUi::entity_components(Entity const entity,
                                gsl::span<ComponentUi> const componentUis)
  -> void {
  if (entity.entity() == entt::null) {
    ImGui::TextUnformatted("no entity selected");

    return;
  }

  constexpr auto nodeFlags =
    ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth;

  // TODO: add a way to show components which don't have a UI
  for (auto const& componentUi : componentUis) {
    auto const* storage = entity.registry()->storage(componentUi.typeId);
    if (!storage) {
      continue;
    }

    if (!storage->contains(entity.entity())) {
      continue;
    }

    if (ImGui::TreeNodeEx(componentUi.name.c_str(), nodeFlags)) {
      componentUi.render(entity);

      ImGui::TreePop();
    }
  }
}

auto DebugUi::transform(Transform& transform) -> void {
  ImGui::DragFloat3("Position", transform.position.components.data(), 0.1f);

  ImGui::DragFloat3("Rotation", transform.rotation.components.data(), 0.01f,
                    -PI, PI);

  ImGui::DragFloat3("Scale", transform.scale.components.data(), 0.1f, 0.0f);
}

auto DebugUi::local_to_world(LocalToWorld const& localToWorld) -> void {
  display_matrix4x4("##value", localToWorld.matrix);
}

auto DebugUi::camera(gfx::Camera& camera) -> void {
  ImGui::DragFloat3("Look At", camera.lookAt.components.data(), 0.1f);
  ImGui::DragFloat3("Up", camera.up.components.data(), 0.1f);
  auto fovRad = camera.fov.radians();
  ImGui::SliderAngle("fov", &fovRad, 1, 179);
  camera.fov = Angle::radians(fovRad);
  ImGui::BeginDisabled();
  ImGui::DragFloat("Aspect Ratio", &camera.aspectRatio);
  ImGui::EndDisabled();
  ImGui::DragFloatRange2("Near and far plane", &camera.nearPlane,
                         &camera.farPlane);
}

auto DebugUi::render_component(gfx::RenderComponent const& rc) -> void {
  ImGui::Text("Mesh: %#x", rc.mesh.value());
  ImGui::Text("Material: %#x", rc.material.value());
}

auto DebugUi::x_model(gfx::ext::XModel const& xModel) -> void {
  ImGui::Text("handle = %d", xModel.value());
}

auto DebugUi::light(gfx::Light& light) -> void {
  std::visit(
    [&](auto&& l) {
      using T = std::decay_t<decltype(l)>;
      if constexpr (std::is_same_v<T, gfx::PointLight>) {
        point_light(l);
      } else if constexpr (std::is_same_v<T, gfx::SpotLight>) {
        spot_light(l);
      } else {
        static_assert(std::is_same_v<T, void>, "non-exhaustive visitor");
      }
    },
    light);
}

auto DebugUi::point_light(gfx::PointLight& light) -> void {
  ImGui::TextUnformatted("Point Light");
  edit_color3("Diffuse", light.diffuse);
  edit_color3("Specular", light.specular);
  edit_color3("Ambient", light.ambient);
  ImGui::DragFloat("Range", &light.range);
  ImGui::DragFloat("Attenuation 0", &light.attenuation0);
  ImGui::DragFloat("Attenuation 1", &light.attenuation1);
  ImGui::DragFloat("Attenuation 2", &light.attenuation2);
}

auto DebugUi::spot_light(gfx::SpotLight& light) -> void {
  ImGui::TextUnformatted("Spot Light");
  edit_color3("Diffuse", light.diffuse);
  edit_color3("Specular", light.specular);
  edit_color3("Ambient", light.ambient);
  ImGui::DragFloat3("Direction", light.direction.components.data());
  ImGui::DragFloat("Range", &light.range);
  ImGui::DragFloat("Attenuation 0", &light.attenuation0);
  ImGui::DragFloat("Attenuation 1", &light.attenuation1);
  ImGui::DragFloat("Attenuation 2", &light.attenuation2);
  ImGui::DragFloat("Falloff", &light.falloff);

  auto phiRad = light.phi.radians();
  ImGui::SliderAngle("Phi", &phiRad, 0, 90);
  light.phi = Angle::radians(phiRad);

  auto thetaRad = light.theta.radians();
  ImGui::SliderAngle("Theta", &thetaRad, 0, light.phi.degrees());
  light.theta = Angle::radians(thetaRad);
}

auto DebugUi::edit_directional_light(gfx::DirectionalLight& light) -> void {
  edit_color4("Diffuse", light.diffuse);
  edit_color4("Specular", light.specular);
  edit_color4("Ambient", light.ambient);

  ImGui::DragFloat3("Direction", light.directionInWorld.components.data(),
                    0.1f);
  light.directionInWorld = Vector3f32::normalize(light.directionInWorld);
}

auto DebugUi::edit_color3(char const* label, Color& color) -> void {
  auto colorArray = array{color.r(), color.g(), color.b()};

  ImGui::ColorEdit3(label, colorArray.data(), ImGuiColorEditFlags_Float);

  color = Color::from_non_linear(
    std::get<0>(colorArray), std::get<1>(colorArray), std::get<2>(colorArray));
}

auto DebugUi::edit_color4(char const* label, Color& color) -> void {
  auto colorArray = array{color.r(), color.g(), color.b(), color.a()};

  ImGui::ColorEdit4(label, colorArray.data(), ImGuiColorEditFlags_Float);

  color =
    Color::from_non_linear(std::get<0>(colorArray), std::get<1>(colorArray),
                           std::get<2>(colorArray), std::get<3>(colorArray));
}

auto DebugUi::display_matrix4x4(char const* label, Matrix4x4f32 const& mat)
  -> void {
  auto const labelString = string{label};

  ImGui::BeginDisabled();

  auto firstRow = array{mat.m11, mat.m12, mat.m13, mat.m14};
  ImGui::InputFloat4((labelString + " Row 1"s).c_str(), firstRow.data(), "%.3f",
                     ImGuiInputTextFlags_ReadOnly);

  auto secondRow = array{mat.m21, mat.m22, mat.m23, mat.m24};
  ImGui::InputFloat4((labelString + " Row 2"s).c_str(), secondRow.data(),
                     "%.3f", ImGuiInputTextFlags_ReadOnly);

  auto thirdRow = array{mat.m31, mat.m32, mat.m33, mat.m34};
  ImGui::InputFloat4((labelString + " Row 3"s).c_str(), thirdRow.data(), "%.3f",
                     ImGuiInputTextFlags_ReadOnly);

  auto forthRow = array{mat.m41, mat.m42, mat.m43, mat.m44};
  ImGui::InputFloat4((labelString + " Row 4"s).c_str(), forthRow.data(), "%.3f",
                     ImGuiInputTextFlags_ReadOnly);

  ImGui::EndDisabled();
}

} // namespace basalt
