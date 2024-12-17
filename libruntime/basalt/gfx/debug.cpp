#include <basalt/gfx/debug.h>

#include <basalt/gfx/backend/commands.h>

#include <basalt/api/debug_ui.h>

#include <basalt/api/gfx/backend/command_list.h>

#include <basalt/api/shared/color.h>

#include <basalt/api/math/vector3.h>

#include <basalt/api/base/types.h>

#include <imgui.h>

#include <array>
#include <string>
#include <utility>

using std::array;
using std::string;
using namespace std::literals;

namespace basalt::gfx {

namespace {

auto sShowCompositeDebugUi = false;

#define ENUM_TO_STRING(e)                                                      \
  case e:                                                                      \
    return #e

#define ENUMERATOR_TO_STRING(e, v)                                             \
  case e::v:                                                                   \
    return #v

constexpr auto enumerator_to_string(CommandType const type) noexcept
  -> char const* {
  switch (type) {
    ENUMERATOR_TO_STRING(CommandType, ClearAttachments);
    ENUMERATOR_TO_STRING(CommandType, Draw);
    ENUMERATOR_TO_STRING(CommandType, DrawIndexed);
    ENUMERATOR_TO_STRING(CommandType, BindPipeline);
    ENUMERATOR_TO_STRING(CommandType, BindVertexBuffer);
    ENUMERATOR_TO_STRING(CommandType, BindIndexBuffer);
    ENUMERATOR_TO_STRING(CommandType, BindTexture);
    ENUMERATOR_TO_STRING(CommandType, BindSampler);
    ENUMERATOR_TO_STRING(CommandType, SetStencilReference);
    ENUMERATOR_TO_STRING(CommandType, SetStencilReadMask);
    ENUMERATOR_TO_STRING(CommandType, SetStencilWriteMask);
    ENUMERATOR_TO_STRING(CommandType, SetBlendConstant);
    ENUMERATOR_TO_STRING(CommandType, SetLights);
    ENUMERATOR_TO_STRING(CommandType, SetTransform);
    ENUMERATOR_TO_STRING(CommandType, SetAmbientLight);
    ENUMERATOR_TO_STRING(CommandType, SetMaterial);
    ENUMERATOR_TO_STRING(CommandType, SetFogParameters);
    ENUMERATOR_TO_STRING(CommandType, SetReferenceAlpha);
    ENUMERATOR_TO_STRING(CommandType, SetTextureFactor);
    ENUMERATOR_TO_STRING(CommandType, SetTextureStageConstant);
    ENUMERATOR_TO_STRING(CommandType, ExtDrawXMesh);
    ENUMERATOR_TO_STRING(CommandType, ExtRenderDearImGui);
    ENUMERATOR_TO_STRING(CommandType, ExtBeginEffect);
    ENUMERATOR_TO_STRING(CommandType, ExtEndEffect);
    ENUMERATOR_TO_STRING(CommandType, ExtBeginEffectPass);
    ENUMERATOR_TO_STRING(CommandType, ExtEndEffectPass);
  }

  return "(unknown)";
}

constexpr auto to_string(Attachment const attachment) noexcept -> char const* {
  switch (attachment) {
    ENUM_TO_STRING(Attachment::RenderTarget);
    ENUM_TO_STRING(Attachment::DepthBuffer);
    ENUM_TO_STRING(Attachment::StencilBuffer);
  }

  return "(unknown)";
}

constexpr auto to_string(CullMode const mode) noexcept -> char const* {
  switch (mode) {
    ENUM_TO_STRING(CullMode::None);
    ENUM_TO_STRING(CullMode::Clockwise);
    ENUM_TO_STRING(CullMode::CounterClockwise);
  }

  return "(unknown)";
}

constexpr auto to_string(TestPassCond const func) noexcept -> char const* {
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

constexpr auto to_string(FillMode const mode) noexcept -> char const* {
  switch (mode) {
    ENUM_TO_STRING(FillMode::Point);
    ENUM_TO_STRING(FillMode::Wireframe);
    ENUM_TO_STRING(FillMode::Solid);
  }

  return "(unknown)";
}

constexpr auto to_string(PrimitiveType const primitiveType) noexcept
  -> char const* {
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

constexpr auto to_string(ShadeMode const mode) noexcept -> char const* {
  switch (mode) {
    ENUM_TO_STRING(ShadeMode::Flat);
    ENUM_TO_STRING(ShadeMode::Gouraud);
  }

  return "(unknown)";
}

constexpr auto to_string(TransformState const state) noexcept -> char const* {
  switch (state) {
    ENUM_TO_STRING(TransformState::ViewToClip);
    ENUM_TO_STRING(TransformState::WorldToView);
    ENUM_TO_STRING(TransformState::LocalToWorld);
    ENUM_TO_STRING(TransformState::Texture0);
    ENUM_TO_STRING(TransformState::Texture1);
    ENUM_TO_STRING(TransformState::Texture2);
    ENUM_TO_STRING(TransformState::Texture3);
    ENUM_TO_STRING(TransformState::Texture4);
    ENUM_TO_STRING(TransformState::Texture5);
    ENUM_TO_STRING(TransformState::Texture6);
    ENUM_TO_STRING(TransformState::Texture7);
  }

  return "(unknown)";
}

#undef ENUMERATOR_TO_STRING
#undef ENUM_TO_STRING

auto display_vec3(char const* label, Vector3f32 const& vec) -> void {
  auto vecArr = array<f32, 3>{vec.x(), vec.y(), vec.z()};

  ImGui::InputFloat3(label, vecArr.data(), "%.3f",
                     ImGuiInputTextFlags_ReadOnly);
}

auto display_color4(char const* label, Color const& color) -> void {
  auto colorArray = array{color.r(), color.g(), color.b(), color.a()};

  ImGui::ColorEdit4(label, colorArray.data(),
                    ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoPicker |
                      ImGuiColorEditFlags_NoInputs |
                      ImGuiColorEditFlags_NoDragDrop);
}

auto display(CommandClearAttachments const& cmd) -> void {
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

auto display(CommandDraw const& cmd) -> void {
  ImGui::Text("firstVertex = %u", cmd.firstVertex);
  ImGui::Text("vertexCount = %u", cmd.vertexCount);
}

auto display(CommandDrawIndexed const& cmd) -> void {
  ImGui::Text("vertexOffset = %i", cmd.vertexOffset);
  ImGui::Text("minIndex = %u", cmd.minIndex);
  ImGui::Text("numVertices = %u", cmd.numVertices);
  ImGui::Text("firstIndex = %u", cmd.firstIndex);
  ImGui::Text("indexCount = %u", cmd.indexCount);
}

auto display(CommandBindPipeline const& cmd) -> void {
  ImGui::Text("pipelineId = %#x", cmd.pipelineId.value());
}

auto display(CommandBindVertexBuffer const& cmd) -> void {
  ImGui::Text("vertexBufferId = %#x", cmd.vertexBufferId.value());
  ImGui::Text("offsetInBytes = %llu", cmd.offsetInBytes);
}

auto display(CommandBindIndexBuffer const& cmd) -> void {
  ImGui::Text("indexBufferId = %#x", cmd.indexBufferId.value());
}

auto display(CommandBindSampler const& cmd) -> void {
  ImGui::Text("slot = %u", cmd.slot);
  ImGui::Text("samplerId = %#x", cmd.samplerId.value());
}

auto display(CommandBindTexture const& cmd) -> void {
  ImGui::Text("slot = %u", cmd.slot);
  ImGui::Text("textureId = %#x", cmd.textureId.value());
}

auto display(CommandSetStencilReference const& cmd) {
  ImGui::Text("value = %u", cmd.value);
}

auto display(CommandSetStencilReadMask const& cmd) {
  ImGui::Text("value = %u", cmd.value);
}

auto display(CommandSetStencilWriteMask const& cmd) {
  ImGui::Text("value = %u", cmd.value);
}

auto display(CommandSetBlendConstant const& cmd) {
  display_color4("value", cmd.value);
}

auto display(PointLightData const& light) -> void {
  ImGui::SeparatorText("PointLightData");

  display_color4("diffuse", light.diffuse);
  display_color4("specular", light.specular);
  display_color4("ambient", light.ambient);

  display_vec3("position", light.positionInWorld);

  auto f = light.rangeInWorld;
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

auto display(SpotLightData const& light) -> void {
  ImGui::SeparatorText("SpotLightData");

  display_color4("diffuse", light.diffuse);
  display_color4("specular", light.specular);
  display_color4("ambient", light.ambient);

  display_vec3("position", light.positionInWorld);
  display_vec3("direction", light.directionInWorld);

  auto f = light.rangeInWorld;
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
  f = light.phi.radians();
  ImGui::InputFloat("phi", &f, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);
  f = light.theta.radians();
  ImGui::InputFloat("theta", &f, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);
}

auto display(DirectionalLightData const& light) -> void {
  ImGui::SeparatorText("DirectionalLightData");
  display_color4("diffuse", light.diffuse);
  display_color4("specular", light.specular);
  display_color4("ambient", light.ambient);

  display_vec3("direction", light.directionInWorld);
}

auto display(CommandSetLights const& cmd) -> void {
  for (auto const& l : cmd.lights) {
    ImGui::PushID(&l);
    visit([](auto&& light) { display(light); }, l);
    ImGui::PopID();
  }
}

auto display(CommandSetAmbientLight const& cmd) -> void {
  display_color4("color", cmd.ambient);
}

auto display(CommandSetTransform const& cmd) -> void {
  ImGui::Text("transformState = %s", to_string(cmd.transformState));
  DebugUi::display_matrix4x4("##transform", cmd.transform);
}

auto display(CommandSetMaterial const& cmd) -> void {
  display_color4("diffuse", cmd.diffuse);
  display_color4("ambient", cmd.ambient);
  display_color4("emissive", cmd.emissive);
  display_color4("specular", cmd.emissive);
  ImGui::Text("specularPower = %.3f", static_cast<f64>(cmd.specularPower));
}

auto display(CommandSetFogParameters const& cmd) -> void {
  display_color4("color", cmd.color);
  auto f = cmd.start;
  ImGui::InputFloat("start", &f, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);
  f = cmd.end;
  ImGui::InputFloat("end", &f, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);
  f = cmd.density;
  ImGui::InputFloat("density", &f, 0, 0, "%.3f", ImGuiInputTextFlags_ReadOnly);
}

auto display(CommandSetReferenceAlpha const& cmd) -> void {
  ImGui::Text("value = %u", cmd.value);
}

auto display(CommandSetTextureFactor const& cmd) -> void {
  display_color4("texture factor", cmd.textureFactor);
}

auto display(CommandSetTextureStageConstant const& cmd) -> void {
  ImGui::Text("stageId = %u", cmd.stageId);
  display_color4("texture factor", cmd.constant);
}

auto display(ext::CommandDrawXMesh const& cmd) -> void {
  ImGui::Text("mesh = %#x", cmd.xMeshId.value());
}

auto display(ext::CommandBeginEffect const& cmd) {
  ImGui::Text("effect = 0x%x", cmd.effect.value());
}

auto display(ext::CommandBeginEffectPass const& cmd) {
  ImGui::Text("passIndex = %u", cmd.passIndex);
}

auto display(Command const& cmd) -> void {
  switch (cmd.type) {
  case CommandType::ExtDrawXMesh:
    display(cmd.as<ext::CommandDrawXMesh>());
    break;

  case CommandType::ExtBeginEffect:
    display(cmd.as<ext::CommandBeginEffect>());
    break;

  case CommandType::ExtBeginEffectPass:
    display(cmd.as<ext::CommandBeginEffectPass>());
    break;

  case CommandType::ExtRenderDearImGui:
  case CommandType::ExtEndEffect:
  case CommandType::ExtEndEffectPass:
    break;

  default:
    BASALT_CRASH("debug can't handle this command");
  }
}

auto draw_composite_inspector(Composite const& composite) -> void {
  ImGui::SetNextWindowSize(ImVec2{500, 350}, ImGuiCond_FirstUseEver);
  if (!ImGui::Begin("Composite Inspector", &sShowCompositeDebugUi)) {
    ImGui::End();
    
    return;
  }

  if (!ImGui::BeginChild("commands", ImVec2{200, 0})) {
    ImGui::EndChild();
    ImGui::End();

    return;
  }

  // TODO: Fix ImGuiCond_Appearing has the same effect as Once
  ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);

  auto id = i32{0};
  Command const* hoveredCommand = {};
  for (auto const& cmdList : composite) {
    ImGui::PushID(id++);

    if (ImGui::TreeNode("Part", "Command List (%llu commands)",
                        cmdList.size())) {
      for (auto const* cmd : cmdList) {
        ImGui::TextUnformatted(enumerator_to_string(cmd->type));

        if (ImGui::IsItemHovered()) {
          hoveredCommand = cmd;
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
    ImGui::End();

    return;
  }

  if (hoveredCommand) {
    visit(*hoveredCommand,
          [](auto&& cmd) { display(std::forward<decltype(cmd)>(cmd)); });
  } else {
    ImGui::TextUnformatted("(hover over a command to see data)");
  }

  ImGui::EndChild();
  ImGui::End();
}

} // namespace

auto Debug::update(Composite const& composite) -> void {
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
