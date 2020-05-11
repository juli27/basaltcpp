#include "Matrices.h"

#include <runtime/Prelude.h>

#include <runtime/Engine.h>

#include <runtime/gfx/Camera.h>
#include <runtime/gfx/RenderComponent.h>

#include <runtime/gfx/backend/IRenderer.h>
#include <runtime/gfx/backend/RenderCommand.h>
#include <runtime/gfx/backend/Types.h>

#include <runtime/math/Constants.h>
#include <runtime/math/Vec3.h>

#include <entt/entity/registry.hpp>
#include <imgui/imgui.h>

#include <array>
#include <tuple>

using std::array;

using basalt::TransformComponent;
using basalt::math::PI;
using basalt::math::Vec3f32;
using basalt::gfx::Camera;
using basalt::gfx::RenderComponent;
using basalt::gfx::backend::IRenderer;
using basalt::gfx::backend::PrimitiveType;
using basalt::gfx::backend::RenderFlagCullNone;
using basalt::gfx::backend::RenderFlagDisableLighting;
using basalt::gfx::backend::VertexElement;
using basalt::gfx::backend::VertexLayout;

namespace d3d9_tuts {

Matrices::Matrices(IRenderer* const renderer) {
  mScene->set_background_color(Color {0.0f, 0.0f, 0.0f});

  const Vec3f32 cameraPos(0.0f, 3.0f, -5.0f);
  const Vec3f32 lookAt(0.0f, 0.0f, 0.0f);
  const Vec3f32 up(0.0f, 1.0f, 0.0f);
  mScene->set_camera(Camera(cameraPos, lookAt, up));

  struct Vertex final {
    f32 x, y, z;
    ColorEncoding::A8R8G8B8 color;
  };

  array<Vertex, 3u> vertices {
    Vertex {-1.0f, -1.0f, 0.0f, ColorEncoding::pack_logical_a8r8g8b8(255, 0, 0)}
  , Vertex {1.0f, -1.0f, 0.0f, ColorEncoding::pack_logical_a8r8g8b8(0, 0, 255)}
  , Vertex {0.0f, 1.0f, 0.0f, ColorEncoding::pack_logical_a8r8g8b8(255, 255, 255)}
  };

  const VertexLayout vertexLayout = {
    VertexElement::Position3F32, VertexElement::ColorDiffuse1U32
  };

  auto& entityRegistry = mScene->get_entity_registry();
  auto entity = entityRegistry.create<TransformComponent, RenderComponent>();

  mTriangleEntity = std::get<0>(entity);

  auto& renderComponent = std::get<2>(entity);
  renderComponent.mMesh = renderer->add_mesh(
    vertices.data(), static_cast<i32>(vertices.size()), vertexLayout
  , PrimitiveType::TriangleList);
  renderComponent.mRenderFlags = RenderFlagCullNone | RenderFlagDisableLighting;
}

void Matrices::on_show() {
  set_current_scene(mScene);
}

void Matrices::on_hide() {
}

void Matrices::on_update() {
  const auto deltaTime = static_cast<f32>(basalt::get_delta_time());

  static auto speed = 2.0f;
  const auto radOffsetY = deltaTime * speed * PI;
  auto& transform =
    mScene->get_entity_registry().get<TransformComponent>(mTriangleEntity);
  transform.rotate(0.0f, radOffsetY, 0.0f);

  ImGui::Begin("D3D9 Tutorial 03: Matrices");
  ImGui::SliderFloat("Rotation Speed", &speed, -2.0f, 2.0f);
  ImGui::End();
}

} // namespace d3d9_tuts
