#include <basalt/gfx/debug.h>

#include <basalt/gfx/utils.h>

#include <basalt/gfx/backend/commands.h>

#include <basalt/api/debug_ui.h>

#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <basalt/api/shared/color.h>

#include <basalt/api/math/vector3.h>

#include <basalt/api/base/types.h>

#include <imgui/imgui.h>

#include <algorithm>
#include <array>
#include <string>
#include <utility>

using std::array;
using std::string;
using namespace std::literals;

namespace basalt::gfx {

namespace {

bool sShowCompositeDebugUi {false};

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
    ENUMERATOR_TO_STRING(CommandType, DrawIndexed);
    ENUMERATOR_TO_STRING(CommandType, BindPipeline);
    ENUMERATOR_TO_STRING(CommandType, BindVertexBuffer);
    ENUMERATOR_TO_STRING(CommandType, BindIndexBuffer);
    ENUMERATOR_TO_STRING(CommandType, BindTexture);
    ENUMERATOR_TO_STRING(CommandType, BindSampler);
    ENUMERATOR_TO_STRING(CommandType, SetLights);
    ENUMERATOR_TO_STRING(CommandType, SetTransform);
    ENUMERATOR_TO_STRING(CommandType, SetAmbientLight);
    ENUMERATOR_TO_STRING(CommandType, SetMaterial);
    ENUMERATOR_TO_STRING(CommandType, SetFogParameters);
    ENUMERATOR_TO_STRING(CommandType, ExtDrawXMesh);
    ENUMERATOR_TO_STRING(CommandType, ExtRenderDearImGui);
  }

  return "(unknown)";
}

constexpr auto to_string(const Attachment attachment) noexcept -> const char* {
  switch (attachment) {
    ENUM_TO_STRING(Attachment::RenderTarget);
    ENUM_TO_STRING(Attachment::DepthBuffer);
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

constexpr auto to_string(const TestPassCond func) noexcept -> const char* {
  switch (func) {
    ENUM_TO_STRING(TestPassCond::Never);
    ENUM_TO_STRING(TestPassCond::IfEqual);
    ENUM_TO_STRING(TestPassCond::IfNotEqual);
    ENUM_TO_STRING(TestPassCond::IfLess);
    ENUM_TO_STRING(TestPassCond::IfLessEqual);
    ENUM_TO_STRING(TestPassCond::IfGreater);
    ENUM_TO_STRING(TestPassCond::IfGreaterEqual);
    ENUM_TO_STRING(TestPassCond::Always);
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

constexpr auto to_string(const ShadeMode mode) noexcept -> const char* {
  switch (mode) {
    ENUM_TO_STRING(ShadeMode::Flat);
    ENUM_TO_STRING(ShadeMode::Gouraud);
  }

  return "(unknown)";
}

constexpr auto to_string(const TransformState state) noexcept -> const char* {
  switch (state) {
    ENUM_TO_STRING(TransformState::ViewToClip);
    ENUM_TO_STRING(TransformState::WorldToView);
    ENUM_TO_STRING(TransformState::LocalToWorld);
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

void display_color4(const char* label, const Color& color) {
  array<float, 4> colorArray {color.r(), color.g(), color.b(), color.a()};

  ImGui::ColorEdit4(label, colorArray.data(),
                    ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoPicker |
                      ImGuiColorEditFlags_NoInputs |
                      ImGuiColorEditFlags_NoDragDrop);
}

void display(const CommandClearAttachments& cmd) {
  if (cmd.attachments.has(Attachment::RenderTarget)) {
    ImGui::TextUnformatted(to_string(Attachment::RenderTarget));
    display_color4("color", cmd.color);
  }

  if (cmd.attachments.has(Attachment::DepthBuffer)) {
    ImGui::TextUnformatted(to_string(Attachment::DepthBuffer));
    ImGui::Text("depth = %#g", static_cast<f64>(cmd.depth));
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

void display(const CommandDrawIndexed& cmd) {
  ImGui::Text("vertexOffset = %i", cmd.vertexOffset);
  ImGui::Text("minIndex = %u", cmd.minIndex);
  ImGui::Text("numVertices = %u", cmd.numVertices);
  ImGui::Text("firstIndex = %u", cmd.firstIndex);
  ImGui::Text("indexCount = %u", cmd.indexCount);
}

void display(const CommandBindPipeline& cmd) {
  ImGui::Text("pipelineId = %#x", cmd.pipelineId.value());
}

void display(const CommandBindVertexBuffer& cmd) {
  ImGui::Text("vertexBufferId = %#x", cmd.vertexBufferId.value());
  ImGui::Text("offsetInBytes = %llu", cmd.offsetInBytes);
}

void display(const CommandBindIndexBuffer& cmd) {
  ImGui::Text("indexBufferId = %#x", cmd.indexBufferId.value());
}

void display(const CommandBindSampler& cmd) {
  ImGui::Text("samplerId = %#x", cmd.samplerId.value());
}

void display(const CommandBindTexture& cmd) {
  ImGui::Text("textureId = %#x", cmd.textureId.value());
}

auto display(const PointLight& light) -> void {
  ImGui::SeparatorText("PointLight");

  display_color4("diffuse", light.diffuse);
  display_color4("specular", light.specular);
  display_color4("ambient", light.ambient);

  display_vec3("position", light.positionInWorld);

  float f {light.rangeInWorld};
  ImGui::InputFloat("range", &f, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);
  f = light.attenuation0;
  ImGui::InputFloat("attenuation0", &f, 0, 0, "%.3f",
                    ImGuiInputTextFlags_ReadOnly);
  f = light.attenuation1;
  ImGui::InputFloat("attenuation1", &f, 0, 0, "%.3f",
                    ImGuiInputTextFlags_ReadOnly);
  f = light.attenuation2;
  ImGui::InputFloat("attenuation2", &f, 0, 0, "%.3f",
                    ImGuiInputTextFlags_ReadOnly);
}

auto display(const SpotLight& light) -> void {
  ImGui::SeparatorText("SpotLight");

  display_color4("diffuse", light.diffuse);
  display_color4("specular", light.specular);
  display_color4("ambient", light.ambient);

  display_vec3("position", light.positionInWorld);
  display_vec3("direction", light.directionInWorld);

  float f {light.rangeInWorld};
  ImGui::InputFloat("range", &f, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);
  f = light.attenuation0;
  ImGui::InputFloat("attenuation0", &f, 0, 0, "%.3f",
                    ImGuiInputTextFlags_ReadOnly);
  f = light.attenuation1;
  ImGui::InputFloat("attenuation1", &f, 0, 0, "%.3f",
                    ImGuiInputTextFlags_ReadOnly);
  f = light.attenuation2;
  ImGui::InputFloat("attenuation2", &f, 0, 0, "%.3f",
                    ImGuiInputTextFlags_ReadOnly);
  f = light.falloff;
  ImGui::InputFloat("falloff", &f, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);
  f = light.phi;
  ImGui::InputFloat("phi", &f, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);
  f = light.theta;
  ImGui::InputFloat("theta", &f, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);
}

auto display(const DirectionalLight& light) -> void {
  ImGui::SeparatorText("DirectionalLight");
  display_color4("diffuse", light.diffuse);
  display_color4("specular", light.specular);
  display_color4("ambient", light.ambient);

  display_vec3("direction", light.directionInWorld);
}

auto display(const CommandSetLights& cmd) -> void {
  for (const Light& l : cmd.lights) {
    ImGui::PushID(&l);
    visit([](auto&& light) { display(light); }, l);
    ImGui::PopID();
  }
}

void display(const CommandSetAmbientLight& cmd) {
  display_color4("color", cmd.ambient);
}

void display(const CommandSetTransform& cmd) {
  ImGui::Text("transformState = %s", to_string(cmd.transformState));
  DebugUi::display_mat4("##transform", cmd.transform);
}

void display(const CommandSetMaterial& cmd) {
  display_color4("diffuse", cmd.diffuse);
  display_color4("ambient", cmd.ambient);
  display_color4("emissive", cmd.emissive);
  display_color4("specular", cmd.emissive);
  ImGui::Text("specularPower = %.3f", static_cast<f64>(cmd.specularPower));
}

auto display(const CommandSetFogParameters& cmd) -> void {
  display_color4("color", cmd.color);
  float f {cmd.start};
  ImGui::InputFloat("start", &f, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);
  f = cmd.end;
  ImGui::InputFloat("end", &f, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);
  f = cmd.density;
  ImGui::InputFloat("density", &f, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);
}

void display(const ext::CommandDrawXMesh& cmd) {
  ImGui::Text("mesh = %#x", cmd.xMeshId.value());
  ImGui::Text("subsetIndex = %#x", cmd.subset);
}

void display(const Command& cmd) {
  switch (cmd.type) {
  case CommandType::ExtDrawXMesh:
    display(cmd.as<ext::CommandDrawXMesh>());
    break;

  case CommandType::ExtRenderDearImGui:
    break;

  default:
    BASALT_CRASH("debug can't handle this command");
  }
}

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
                        cmdList.size())) {
      std::for_each(cmdList.begin(), cmdList.end(), [&](const Command* cmd) {
        ImGui::TextUnformatted(enumerator_to_string(cmd->type));

        if (ImGui::IsItemHovered()) {
          hoveredCommand = cmd;
        }
      });

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
    visit(*hoveredCommand,
          [](auto&& cmd) { display(std::forward<decltype(cmd)>(cmd)); });
  }

  ImGui::EndChild();
  ImGui::End();
}

} // namespace

void Debug::update(const Composite& composite) {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("View")) {
      ImGui::MenuItem("Composite Inspector", nullptr, &sShowCompositeDebugUi);

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }

  if (sShowCompositeDebugUi) {
    draw_composite_inspector(composite);
  }
}

} // namespace basalt::gfx
