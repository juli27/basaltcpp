#include <basalt/api/debug_ui.h>

#include "gfx/utils.h"

#include <basalt/api/gfx/info.h>

#include <basalt/api/shared/color.h>

#include <basalt/api/math/matrix4x4.h>
#include <basalt/api/math/vector3.h>

#include <basalt/api/base/enum_array.h>

#include <fmt/format.h>
#include <imgui.h>

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

auto to_string(gfx::BackendApi const api) -> string_view {
  static constexpr auto TO_STRING = EnumArray<gfx::BackendApi, string_view, 2>{
    {gfx::BackendApi::Default, "Default"sv},
    {gfx::BackendApi::Direct3D9, "Direct3D 9"sv},
  };
  static_assert(gfx::BACKEND_API_COUNT == TO_STRING.size());

  return TO_STRING[api];
}

auto to_string(gfx::DisplayMode const& mode) noexcept -> string {
  auto const gcd = std::gcd(mode.width, mode.height);

  return fmt::format(FMT_STRING("{}x{} ({}:{}) {}Hz"), mode.width, mode.height,
                     mode.width / gcd, mode.height / gcd, mode.refreshRate);
}

} // namespace

auto DebugUi::show_gfx_info(gfx::Info const& gfxInfo) -> void {
  auto const& selectedAdapter = gfxInfo.adapterInfos[mSelectedAdapterIndex];

  ImGui::Text("Backend API: %s", to_string(gfxInfo.backendApi).data());

  ImGui::Separator();

  auto const& adapterIdentifier = selectedAdapter.identifier;

  if (ImGui::BeginCombo("Adapter", adapterIdentifier.displayName.c_str())) {
    for (auto const& adapter : gfxInfo.adapterInfos) {
      ImGui::PushID(&adapter);

      auto const adapterIndex = adapter.index;
      auto const isSelected = adapterIndex == mSelectedAdapterIndex;

      if (ImGui::Selectable(adapterIdentifier.displayName.c_str(),
                            isSelected)) {
        mSelectedAdapterIndex = adapterIndex;
      }

      if (isSelected) {
        ImGui::SetItemDefaultFocus();
      }

      ImGui::PopID();
    }

    ImGui::EndCombo();
  }

  ImGui::Text("Driver: %s", adapterIdentifier.driverInfo.c_str());

  ImGui::Separator();

  ImGui::TextUnformatted("Adapter Modes");

  auto const& sharedModeInfo = selectedAdapter.sharedModeInfo;
  auto const& exclusiveModes = selectedAdapter.exclusiveModes;

  if (ImGui::IsWindowAppearing()) {
    for (auto i = uSize{}; i < exclusiveModes.size(); i++) {
      if (exclusiveModes[i].displayFormat == sharedModeInfo.displayFormat) {
        mSelectedModeIndex = i;
      }
    }
  }

  auto const& sharedDisplayMode = sharedModeInfo.displayMode;
  auto const& selectedMode = exclusiveModes[mSelectedModeIndex];
  if (ImGui::BeginCombo("Display Format",
                        to_string(selectedMode.displayFormat))) {
    for (auto i = uSize{}; i < exclusiveModes.size(); i++) {
      auto const& mode = exclusiveModes[i];
      auto const isSelected = i == mSelectedModeIndex;

      if (ImGui::Selectable(to_string(mode.displayFormat), isSelected)) {
        mSelectedModeIndex = i;
      }

      if (isSelected) {
        ImGui::SetItemDefaultFocus();
      }
    }

    ImGui::EndCombo();
  }

  if (ImGui::BeginChild("modes")) {
    for (auto const& displayMode : selectedMode.displayModes) {
      if (selectedMode.displayFormat == sharedModeInfo.displayFormat &&
          displayMode.width == sharedDisplayMode.width &&
          displayMode.height == sharedDisplayMode.height &&
          displayMode.refreshRate == sharedDisplayMode.refreshRate) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{1.0f, 1.0f, 0.0f, 1.0f});
        ImGui::TextUnformatted(to_string(displayMode).c_str());
        ImGui::PopStyleColor();

        if (ImGui::IsWindowAppearing()) {
          ImGui::SetScrollHereY();
        }
      } else {
        ImGui::TextUnformatted(to_string(displayMode).c_str());
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

auto DebugUi::edit_directional_light(gfx::DirectionalLight& light) -> void {
  edit_color4("Diffuse", light.diffuse);
  edit_color4("Specular", light.specular);
  edit_color4("Ambient", light.ambient);

  ImGui::DragFloat3("Direction", light.directionInWorld.components.data(),
                    0.1f);
  light.directionInWorld = light.directionInWorld.normalize();
}

auto DebugUi::edit_color3(char const* label, Color& color) -> bool {
  auto colorArray = array{color.r(), color.g(), color.b()};
  if (!ImGui::ColorEdit3(label, colorArray.data(), ImGuiColorEditFlags_Float)) {
    return false;
  }

  color = Color::from_non_linear(
    std::get<0>(colorArray), std::get<1>(colorArray), std::get<2>(colorArray));

  return true;
}

auto DebugUi::edit_color4(char const* label, Color& color) -> bool {
  auto colorArray = array{color.r(), color.g(), color.b(), color.a()};
  if (!ImGui::ColorEdit4(label, colorArray.data(), ImGuiColorEditFlags_Float)) {
    return false;
  }

  color =
    Color::from_non_linear(std::get<0>(colorArray), std::get<1>(colorArray),
                           std::get<2>(colorArray), std::get<3>(colorArray));

  return true;
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
