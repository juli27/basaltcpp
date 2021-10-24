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

bool sShowPerformanceOverlay {true};
bool sShowCompositeDebugUi {false};

void draw_overlay() {
  static i8 corner {2};

  constexpr f32 distanceToEdge {8.0f};
  const ImGuiViewport& vp {*ImGui::GetMainViewport()};

  const f32 windowPosX {corner & 0x1
                          ? vp.WorkPos.x + vp.WorkSize.x - distanceToEdge
                          : vp.WorkPos.x + distanceToEdge};

  const f32 windowPosY {corner & 0x2
                          ? vp.WorkPos.y + vp.WorkSize.y - distanceToEdge
                          : vp.WorkPos.y + distanceToEdge};

  const ImVec2 windowPosPivot {corner & 0x1 ? 1.0f : 0.0f,
                               corner & 0x2 ? 1.0f : 0.0f};

  ImGui::SetNextWindowPos(ImVec2 {windowPosX, windowPosY}, ImGuiCond_Always,
                          windowPosPivot);
  ImGui::SetNextWindowBgAlpha(0.35f);

  constexpr ImGuiWindowFlags flags {
    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration |
    ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
    ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav};

  if (ImGui::Begin("Overlay", &sShowPerformanceOverlay, flags)) {
    const ImGuiIO& io {ImGui::GetIO()};

    const f64 fps {io.Framerate};

    ImGui::Text("%.3f ms/frame (%.1f fps)", 1000.0 / fps, fps);

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
      if (ImGui::MenuItem("Hide")) {
        sShowPerformanceOverlay = false;
      }

      ImGui::EndPopup();
    }
  }

  ImGui::End();
}

void draw_gfx_info_modal(const Info& info) {
  if (ImGui::BeginPopupModal("Gfx Info", nullptr,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    static u32 currentIndex {0};
    const AdapterInfo& current {info.adapters[currentIndex]};
    if (ImGui::BeginCombo("GFX Adapter", current.displayName.c_str())) {
      for (const auto& adapter : info.adapters) {
        const bool isSelected {adapter.adapterIndex == currentIndex};
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
      for (const auto& mode : current.adapterModes) {
        ImGui::Selectable(to_string(mode).c_str(), false,
                          ImGuiSelectableFlags_DontClosePopups);
      }
    }

    ImGui::EndChild();

    if (ImGui::Button("Close", ImVec2 {120.0f, 0.0f})) {
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }
}

#define ENUM_TO_STRING(e)                                                      \
  case e:                                                                      \
    return #e

#define ENUMERATOR_TO_STRING(e, v)                                             \
  case e::v:                                                                   \
    return #v

constexpr auto enumerator_to_string(const CommandType type) noexcept -> const
  char* {
  switch (type) {
    ENUMERATOR_TO_STRING(CommandType, ClearAttachments);
    ENUMERATOR_TO_STRING(CommandType, Draw);
    ENUMERATOR_TO_STRING(CommandType, SetRenderState);
    ENUMERATOR_TO_STRING(CommandType, BindPipeline);
    ENUMERATOR_TO_STRING(CommandType, BindVertexBuffer);
    ENUMERATOR_TO_STRING(CommandType, BindTexture);
    ENUMERATOR_TO_STRING(CommandType, BindSampler);
    ENUMERATOR_TO_STRING(CommandType, SetTextureStageState);
    ENUMERATOR_TO_STRING(CommandType, SetDirectionalLights);
    ENUMERATOR_TO_STRING(CommandType, SetTransform);
    ENUMERATOR_TO_STRING(CommandType, SetMaterial);
    ENUMERATOR_TO_STRING(CommandType, ExtDrawXModel);
    ENUMERATOR_TO_STRING(CommandType, ExtRenderDearImGui);
  }

  return "(unknown)";
}

constexpr auto to_string(const Attachment attachment) noexcept -> const char* {
  switch (attachment) {
    ENUM_TO_STRING(Attachment::Color);
    ENUM_TO_STRING(Attachment::ZBuffer);
    ENUM_TO_STRING(Attachment::StencilBuffer);
  }

  return "(unknown)";
}

constexpr auto to_string(const CullMode mode) noexcept -> const char* {
  switch (mode) {
    ENUM_TO_STRING(CullMode::None);
    ENUM_TO_STRING(CullMode::Clockwise);
    ENUM_TO_STRING(CullMode::CounterClockwise);
  }

  return "(unknown)";
}

constexpr auto to_string(const DepthTestPass func) noexcept -> const char* {
  switch (func) {
    ENUM_TO_STRING(DepthTestPass::Never);
    ENUM_TO_STRING(DepthTestPass::IfEqual);
    ENUM_TO_STRING(DepthTestPass::IfNotEqual);
    ENUM_TO_STRING(DepthTestPass::IfLess);
    ENUM_TO_STRING(DepthTestPass::IfLessEqual);
    ENUM_TO_STRING(DepthTestPass::IfGreater);
    ENUM_TO_STRING(DepthTestPass::IfGreaterEqual);
    ENUM_TO_STRING(DepthTestPass::Always);
  }

  return "(unknown)";
}

constexpr auto to_string(const FillMode mode) noexcept -> const char* {
  switch (mode) {
    ENUM_TO_STRING(FillMode::Point);
    ENUM_TO_STRING(FillMode::Wireframe);
    ENUM_TO_STRING(FillMode::Solid);
  }

  return "(unknown)";
}

constexpr auto to_string(const PrimitiveType primitiveType) noexcept -> const
  char* {
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

constexpr auto to_string(const RenderStateType state) noexcept -> const char* {
  switch (state) {
    ENUM_TO_STRING(RenderStateType::Ambient);
    ENUM_TO_STRING(RenderStateType::FillMode);
    ENUM_TO_STRING(RenderStateType::ShadeMode);
  }

  return "(unknown)";
}

constexpr auto to_string(const ShadeMode mode) noexcept -> const char* {
  switch (mode) {
    ENUM_TO_STRING(ShadeMode::Flat);
    ENUM_TO_STRING(ShadeMode::Gouraud);
  }

  return "(unknown)";
}

constexpr auto to_string(const TextureStageState state) noexcept -> const
  char* {
  switch (state) {
    ENUM_TO_STRING(TextureStageState::CoordinateSource);
    ENUM_TO_STRING(TextureStageState::TextureTransformFlags);
  }

  return "(unknown)";
}

constexpr auto to_string(const TransformState state) noexcept -> const char* {
  switch (state) {
    ENUM_TO_STRING(TransformState::ViewToViewport);
    ENUM_TO_STRING(TransformState::WorldToView);
    ENUM_TO_STRING(TransformState::ModelToWorld);
    ENUM_TO_STRING(TransformState::Texture);
  }

  return "(unknown)";
}

#undef ENUMERATOR_TO_STRING
#undef ENUM_TO_STRING

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

void display_color4(const char* label, const Color& color) {
  array<float, 4> colorArray {color.red(), color.green(), color.blue(),
                              color.alpha()};

  ImGui::ColorEdit4(label, colorArray.data(),
                    ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoPicker |
                      ImGuiColorEditFlags_NoInputs |
                      ImGuiColorEditFlags_NoDragDrop);
}

void display(const CommandClearAttachments& cmd) {
  if (cmd.attachments.has(Attachment::Color)) {
    ImGui::TextUnformatted(to_string(Attachment::Color));
    display_color4("color", cmd.color);
  }

  if (cmd.attachments.has(Attachment::ZBuffer)) {
    ImGui::TextUnformatted(to_string(Attachment::ZBuffer));
    ImGui::Text("z = %#g", static_cast<f64>(cmd.z));
  }

  if (cmd.attachments.has(Attachment::StencilBuffer)) {
    ImGui::TextUnformatted(to_string(Attachment::StencilBuffer));
    ImGui::Text("stencil = %u", cmd.stencil);
  }
}

void display(const CommandDraw& cmd) {
  ImGui::Text("firstVertex = %u", cmd.firstVertex);
  ImGui::Text("vertexCount = %u", cmd.vertexCount);
}

void display(const CommandSetRenderState& cmd) {
  ImGui::Text("type = %s", to_string(cmd.renderState.type()));

  std::visit(
    [](auto&& value) {
      using T = std::decay_t<decltype(value)>;
      if constexpr (std::is_same_v<T, bool>) {
        ImGui::Text("enabled = %s", value ? "true" : "false");
      } else if constexpr (std::disjunction_v<std::is_same<T, CullMode>,
                                              std::is_same<T, FillMode>,
                                              std::is_same<T, ShadeMode>,
                                              std::is_same<T, DepthTestPass>>) {
        ImGui::TextUnformatted(to_string(value));
      } else if constexpr (std::is_same_v<T, Color>) {
        display_color4("color", value);
      } else {
        static_assert(false, "non-exhaustive visitor");
      }
    },
    cmd.renderState.value());
}

void display(const CommandBindPipeline& cmd) {
  ImGui::Text("handle = %#x", cmd.handle.value());
}

void display(const CommandBindVertexBuffer& cmd) {
  ImGui::Text("handle = %#x", cmd.handle.value());
  ImGui::Text("offset = %llu", cmd.offset);
}

void display(const CommandBindSampler& cmd) {
  ImGui::Text("handle = %#x", cmd.sampler.value());
}

void display(const CommandBindTexture& cmd) {
  ImGui::Text("handle = %#x", cmd.texture.value());
}

void display(const CommandSetTextureStageState& cmd) {
  ImGui::Text("state = %s\nvalue = %#x", to_string(cmd.state), cmd.value);
}

void display(const CommandSetDirectionalLights& cmd) {
  i32 i {0};
  for (const DirectionalLight& light : cmd.directionalLights) {
    ImGui::PushID(i++);

    display_vec3("direction", light.direction);
    display_color4("diffuseColor", light.diffuseColor);
    display_color4("ambientColor", light.ambientColor);

    ImGui::PopID();
  }
}

void display(const CommandSetTransform& cmd) {
  ImGui::Text("state = %s", to_string(cmd.state));
  display_mat4("##transform", cmd.transform);
}

void display(const CommandSetMaterial& cmd) {
  display_color4("diffuseColor", cmd.diffuse);
  display_color4("ambientColor", cmd.ambient);
  display_color4("emissiveColor", cmd.emissive);
}

void display(const ext::CommandDrawXModel& cmd) {
  ImGui::Text("model = %#x", cmd.handle.value());
}

void display(const ext::CommandRenderDearImGui&) {
}

#define DISPLAY(commandStruct)                                                 \
  case commandStruct::TYPE:                                                    \
    display(command->as<commandStruct>());                                     \
    break

void draw_composite_inspector(const Composite& composite) {
  ImGui::SetNextWindowSize(ImVec2 {500, 350}, ImGuiCond_FirstUseEver);
  if (!ImGui::Begin("Composite Inspector", &sShowCompositeDebugUi)) {
    ImGui::End();
    return;
  }

  if (!ImGui::BeginChild("commands", ImVec2 {200, 0})) {
    ImGui::EndChild();
    return;
  }

  // TODO: Fix ImGuiCond_Appearing has the same effect as Once
  ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);

  i32 id {0};
  const Command* hoveredCommand {};
  for (const auto& cmdList : composite) {
    ImGui::PushID(id++);

    if (ImGui::TreeNode("Part", "Command List (%llu commands)",
                        cmdList.commands().size())) {
      for (const auto& command : cmdList.commands()) {
        ImGui::TextUnformatted(enumerator_to_string(command->type));

        if (ImGui::IsItemHovered()) {
          hoveredCommand = command;
        }
      }

      ImGui::TreePop();
    }

    ImGui::PopID();
  }

  ImGui::EndChild();

  ImGui::SameLine();

  if (!ImGui::BeginChild("command data")) {
    ImGui::EndChild();
    return;
  }

  if (hoveredCommand) {
    const Command* command {hoveredCommand};
    switch (command->type) {
      DISPLAY(CommandClearAttachments);
      DISPLAY(CommandDraw);
      DISPLAY(CommandSetRenderState);
      DISPLAY(CommandBindPipeline);
      DISPLAY(CommandBindVertexBuffer);
      DISPLAY(CommandBindSampler);
      DISPLAY(CommandBindTexture);
      DISPLAY(CommandSetTransform);
      DISPLAY(CommandSetDirectionalLights);
      DISPLAY(CommandSetMaterial);
      DISPLAY(CommandSetTextureStageState);

      DISPLAY(ext::CommandDrawXModel);
      DISPLAY(ext::CommandRenderDearImGui);
    }
  }

  ImGui::EndChild();
  ImGui::End();
}

#undef DISPLAY

} // namespace

void Debug::update(const Info& info, const Composite& composite) {
  // https://github.com/ocornut/imgui/issues/331
  enum class OpenPopup : u8 { None, GfxInfo };
  OpenPopup shouldOpenPopup {OpenPopup::None};

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("View")) {
      ImGui::MenuItem("Performance Overlay", nullptr, &sShowPerformanceOverlay);
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

  draw_gfx_info_modal(info);

  if (sShowCompositeDebugUi) {
    draw_composite_inspector(composite);
  }

  if (sShowPerformanceOverlay) {
    draw_overlay();
  }
}

} // namespace basalt::gfx
