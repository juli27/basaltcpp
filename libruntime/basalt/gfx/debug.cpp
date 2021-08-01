#include <basalt/gfx/debug.h>

#include <basalt/gfx/utils.h>

#include <basalt/api/debug.h>

#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/commands.h>
#include <basalt/api/gfx/backend/ext/dear_imgui_renderer.h>
#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <basalt/api/shared/color.h>

#include <basalt/api/math/mat4.h>
#include <basalt/api/math/vector3.h>

#include <basalt/api/base/types.h>
#include <basalt/api/base/utils.h>

#include <fmt/format.h>

#include <imgui/imgui.h>

#include <array>
#include <string>
#include <type_traits>
#include <variant>

using std::array;
using std::string;
using namespace std::literals;

namespace basalt::gfx {

namespace {

bool sShowCompositeDebugUi {false};

void draw_overlay() {
  static i8 corner = 2;

  const f32 distanceToEdge = 8.0f;
  auto* vp = ImGui::GetMainViewport();

  const f32 windowPosX = corner & 0x1
                           ? vp->WorkPos.x + vp->WorkSize.x - distanceToEdge
                           : vp->WorkPos.x + distanceToEdge;

  const f32 windowPosY = corner & 0x2
                           ? vp->WorkPos.y + vp->WorkSize.y - distanceToEdge
                           : vp->WorkPos.y + distanceToEdge;

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
    auto& io = ImGui::GetIO();

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

void display_color4(const char* label, const Color& color) {
  array<float, 4> colorArray {color.red(), color.green(), color.blue(),
                              color.alpha()};

  ImGui::ColorEdit4(label, colorArray.data(),
                    ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoPicker |
                      ImGuiColorEditFlags_NoInputs |
                      ImGuiColorEditFlags_NoDragDrop);
}

void display(const CommandClear& cmd) {
  ImGui::TextUnformatted("Clear");
  if (ImGui::IsItemHovered()) {
    ImGui::BeginTooltip();

    display_color4("color", cmd.color);

    ImGui::EndTooltip();
  }
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

void display_vec3(const char* label, const Vector3f32& vec) {
  array<f32, 3> vecArr {vec.x(), vec.y(), vec.z()};

  ImGui::InputFloat3(label, vecArr.data(), "%.3f",
                     ImGuiInputTextFlags_ReadOnly);
}

void display(const CommandSetDirectionalLights& cmd) {
  ImGui::TextUnformatted("SetDirectionalLights");
  if (ImGui::IsItemHovered()) {
    ImGui::BeginTooltip();

    i32 i = 0;
    for (const auto& light : cmd.directionalLights) {
      ImGui::PushID(i++);

      display_vec3("direction", light.direction);
      display_color4("diffuseColor", light.diffuseColor);
      display_color4("ambientColor", light.ambientColor);

      ImGui::PopID();
    }

    ImGui::EndTooltip();
  }
}

auto to_string(const CullMode mode) -> const char* {
  switch (mode) {
  case CullMode::None:
    return "CullMode::None";

  case CullMode::Clockwise:
    return "CullMode::Clockwise";

  case CullMode::CounterClockwise:
    return "CullMode::CounterClockwise";
  }

  return "(unknown)";
}

auto to_string(const TransformState state) -> const char* {
  switch (state) {
  case TransformState::Projection:
    return "TransformState::Projection";

  case TransformState::View:
    return "TransformState::View";

  case TransformState::World:
    return "TransformState::World";

  case TransformState::Texture:
    return "TransformState::Texture";
  }

  return "(unknown)";
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

void display(const CommandSetTransform& cmd) {
  ImGui::TextUnformatted("SetTransform");
  if (ImGui::IsItemHovered()) {
    ImGui::BeginTooltip();

    ImGui::Text("state = %s", to_string(cmd.state));

    display_mat4("##transform", cmd.transform);

    ImGui::EndTooltip();
  }
}

void display(const CommandSetMaterial& cmd) {
  ImGui::TextUnformatted("SetMaterial");
  if (ImGui::IsItemHovered()) {
    ImGui::BeginTooltip();

    display_color4("diffuseColor", cmd.diffuse);
    display_color4("ambientColor", cmd.ambient);
    display_color4("emissiveColor", cmd.emissive);

    ImGui::EndTooltip();
  }
}

auto to_string(const RenderStateType state) -> const char* {
  switch (state) {
  case RenderStateType::Lighting:
    return "RenderStateType::Lighting";

  case RenderStateType::Ambient:
    return "RenderStateType::Ambient";

  case RenderStateType::CullMode:
    return "RenderStateType::CullMode";

  case RenderStateType::FillMode:
    return "RenderStateType::FillMode";

  case RenderStateType::ShadeMode:
    return "RenderStateType::ShadeMode";
  }

  return "(unknown)";
}

auto to_string(const FillMode mode) -> const char* {
  switch (mode) {
  case FillMode::Point:
    return "FillMode::Point";

  case FillMode::Wireframe:
    return "FillMode::Wireframe";

  case FillMode::Solid:
    return "FillMode::Solid";
  }

  return "(unknown)";
}

auto to_string(const ShadeMode mode) -> const char* {
  switch (mode) {
  case ShadeMode::Flat:
    return "ShadeMode::Flat";

  case ShadeMode::Gouraud:
    return "ShadeMode::Gouraud";
  }

  return "(unknown)";
}

void display(const CommandSetRenderState& cmd) {
  ImGui::TextUnformatted("SetRenderState");
  if (ImGui::IsItemHovered()) {
    ImGui::BeginTooltip();

    ImGui::Text("type = %s", to_string(cmd.renderState.type()));

    std::visit(
      [](auto&& value) {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, bool>) {
          ImGui::Text("value = %s", value ? "true" : "false");
        } else if constexpr (std::disjunction_v<std::is_same<T, CullMode>,
                                                std::is_same<T, FillMode>,
                                                std::is_same<T, ShadeMode>>) {
          ImGui::Text("mode = %s", to_string(value));
        } else if constexpr (std::is_same_v<T, Color>) {
          display_color4("color", value);
        } else {
          static_assert(false, "non-exhaustive visitor");
        }
      },
      cmd.renderState.value());

    ImGui::EndTooltip();
  }
}

void display(const CommandSetTexture& cmd) {
  ImGui::TextUnformatted("SetTexture");
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("handle = %#x", cmd.texture.value());
  }
}

auto to_string(const TextureStageState state) -> const char* {
  constexpr array<const char*, 2> strings {
    "TextureStageState::CoordinateSource",
    "TextureStageState::TextureTransformFlags",
  };
  static_assert(TEXTURE_STAGE_STATE_COUNT == strings.size());

  return strings[enum_cast(state)];
}

void display(const CommandSetTextureStageState& cmd) {
  ImGui::TextUnformatted("SetTextureStageState");
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("state = %s\nvalue = %#x", to_string(cmd.state),
                      cmd.value);
  }
}

void display(const ext::CommandDrawXModel& cmd) {
  ImGui::TextUnformatted("ExtDrawXModel");
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("model = %#x", cmd.handle.value());
  }
}

void display(const ext::CommandRenderDearImGui&) {
  ImGui::TextUnformatted("ExtRenderDearImGui");
}

#define DISPLAY(commandStruct)                                                 \
  case commandStruct::TYPE:                                                    \
    display(command->as<commandStruct>());                                     \
    break

void draw_composite_inspector(const Composite& composite) {
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

          DISPLAY(ext::CommandDrawXModel);
          DISPLAY(ext::CommandRenderDearImGui);
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

} // namespace

void Debug::update(const Info& info, const Composite& composite) {
  draw_overlay();

  // https://github.com/ocornut/imgui/issues/331
  enum class OpenPopup : u8 { None, GfxInfo };
  OpenPopup shouldOpenPopup {OpenPopup::None};

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("View")) {
      ImGui::MenuItem("Composite Inspector", nullptr, &sShowCompositeDebugUi);

      ImGui::Separator();

      if (ImGui::MenuItem("GFX Info...")) {
        shouldOpenPopup = OpenPopup::GfxInfo;
      }

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }

  if (shouldOpenPopup == OpenPopup::GfxInfo) {
    ImGui::OpenPopup("Gfx Info");
  }

  if (ImGui::BeginPopupModal("Gfx Info", nullptr,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    static u32 currentIndex = 0;
    const AdapterInfo& current = info.adapters[currentIndex];
    if (ImGui::BeginCombo("GFX Adapter", current.displayName.c_str())) {
      for (const auto& adapter : info.adapters) {
        const bool isSelected = adapter.adapterIndex == currentIndex;
        if (ImGui::Selectable(
              fmt::format("{} ##{}", adapter.displayName, adapter.adapterIndex)
                .c_str(),
              isSelected)) {
          currentIndex = adapter.adapterIndex;
        }

        if (isSelected) {
          ImGui::SetItemDefaultFocus();
        }
      }

      ImGui::EndCombo();
    }

    ImGui::Text("Driver: %s", current.driverInfo.c_str());

    ImGui::Separator();

    ImGui::TextUnformatted("Adapter Modes");

    if (ImGui::BeginChild("modes", ImVec2 {0, 250})) {
      for (const auto& adapterMode : current.adapterModes) {
        ImGui::Selectable(to_string(adapterMode).c_str(), false,
                          ImGuiSelectableFlags_DontClosePopups);
      }
    }

    ImGui::EndChild();

    if (ImGui::Button("Close", ImVec2 {120.0f, 0.0f})) {
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }

  draw_composite_inspector(composite);
}

} // namespace basalt::gfx
