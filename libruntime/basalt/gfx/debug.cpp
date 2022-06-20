#include <basalt/gfx/debug.h>

#include <basalt/gfx/utils.h>

#include <basalt/gfx/backend/commands.h>
#include <basalt/gfx/backend/types.h>
#include <basalt/gfx/backend/utils.h>

#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <basalt/api/shared/color.h>

#include <basalt/api/math/matrix4x4.h>
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
    ENUMERATOR_TO_STRING(CommandType, BindPipeline);
    ENUMERATOR_TO_STRING(CommandType, BindVertexBuffer);
    ENUMERATOR_TO_STRING(CommandType, BindTexture);
    ENUMERATOR_TO_STRING(CommandType, BindSampler);
    ENUMERATOR_TO_STRING(CommandType, SetLights);
    ENUMERATOR_TO_STRING(CommandType, SetTransform);
    ENUMERATOR_TO_STRING(CommandType, SetAmbientLight);
    ENUMERATOR_TO_STRING(CommandType, SetMaterial);
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

constexpr auto to_string(const TestOp func) noexcept -> const char* {
  switch (func) {
    ENUM_TO_STRING(TestOp::PassNever);
    ENUM_TO_STRING(TestOp::PassIfEqual);
    ENUM_TO_STRING(TestOp::PassIfNotEqual);
    ENUM_TO_STRING(TestOp::PassIfLess);
    ENUM_TO_STRING(TestOp::PassIfLessEqual);
    ENUM_TO_STRING(TestOp::PassIfGreater);
    ENUM_TO_STRING(TestOp::PassIfGreaterEqual);
    ENUM_TO_STRING(TestOp::PassAlways);
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

void display_mat4(const char* label, const Matrix4x4f32& mat) {
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

void display(const PointLight& light) {
  display_vec3("position", light.positionInWorld);
  display_color4("diffuseColor", light.diffuseColor);
  display_color4("ambientColor", light.ambientColor);

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

void display(const SpotLight& light) {
  display_vec3("position", light.positionInWorld);
  display_vec3("direction", light.directionInWorld);
  display_color4("diffuseColor", light.diffuseColor);
  display_color4("ambientColor", light.ambientColor);

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

void display(const DirectionalLight& light) {
  display_vec3("direction", light.direction);
  display_color4("diffuseColor", light.diffuseColor);
  display_color4("ambientColor", light.ambientColor);
}

void display(const CommandSetLights& cmd) {
  i32 i {0};
  for (const Light& l : cmd.lights) {
    ImGui::PushID(i++);
    visit([](auto&& light) { display(light); }, l);
    ImGui::PopID();
  }
}

void display(const CommandSetAmbientLight& cmd) {
  display_color4("color", cmd.ambientColor);
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

void display(const ext::CommandDrawXMesh& cmd) {
  ImGui::Text("mesh = %#x", cmd.handle.value());
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
    ImGui::TextUnformatted("(Unknown)");
    break;
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
