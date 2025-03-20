#include "component_ui.h"

#include <basalt/api/debug_ui.h>

#include <basalt/api/gfx/camera.h>
#include <basalt/api/gfx/types.h>

#include <basalt/api/scene/transform.h>

#include <basalt/api/math/angle.h>

#include <basalt/api/base/functional.h>

#include <imgui.h>

#include <variant>

using namespace basalt;

auto ComponentUi::transform(Transform& transform) -> void {
  ImGui::DragFloat3("Position", transform.position.components.data(), 0.1f);

  ImGui::DragFloat3("Rotation", transform.rotation.components.data(), 0.01f,
                    -PI, PI);

  ImGui::DragFloat3("Scale", transform.scale.components.data(), 0.1f);
}

auto ComponentUi::local_to_world(LocalToWorld const& localToWorld) -> void {
  DebugUi::display_matrix4x4("##value", localToWorld.matrix);
}

auto ComponentUi::camera(gfx::Camera& camera) -> void {
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

auto ComponentUi::model(gfx::Model const& model) -> void {
  ImGui::Text("Material: %#x", model.material.value());
  ImGui::Text("Mesh: %#x", model.mesh.value());
}

auto ComponentUi::x_model(gfx::ext::XModel const& model) -> void {
  ImGui::Text("Material: %#x", model.material.value());
  ImGui::Text("X Mesh: %#x", model.mesh.value());
}

auto ComponentUi::light(gfx::Light& light) -> void {
  std::visit(Overloaded{[&](gfx::PointLight& l) { point_light(l); },
                        [&](gfx::SpotLight& l) { spot_light(l); }},
             light);
}

auto ComponentUi::point_light(gfx::PointLight& light) -> void {
  ImGui::TextUnformatted("Point Light");
  DebugUi::edit_color3("Diffuse", light.diffuse);
  DebugUi::edit_color3("Specular", light.specular);
  DebugUi::edit_color3("Ambient", light.ambient);
  ImGui::DragFloat("Range", &light.range);
  ImGui::DragFloat("Attenuation 0", &light.attenuation0);
  ImGui::DragFloat("Attenuation 1", &light.attenuation1);
  ImGui::DragFloat("Attenuation 2", &light.attenuation2);
}

auto ComponentUi::spot_light(gfx::SpotLight& light) -> void {
  ImGui::TextUnformatted("Spot Light");
  DebugUi::edit_color3("Diffuse", light.diffuse);
  DebugUi::edit_color3("Specular", light.specular);
  DebugUi::edit_color3("Ambient", light.ambient);
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
