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
using basalt::gfx::backend::PrimitiveType;
using basalt::gfx::backend::RenderFlagCullNone;
using basalt::gfx::backend::RenderFlagDisableLighting;
using basalt::gfx::backend::VertexElementType;
using basalt::gfx::backend::VertexElementUsage;
using basalt::gfx::backend::VertexLayout;

namespace d3d9_tuts {

Matrices::Matrices() {
  mScene->set_background_color(Color(0, 0, 0));

  const Vec3f32 cameraPos(0.0f, 3.0f, -5.0f);
  const Vec3f32 lookAt(0.0f, 0.0f, 0.0f);
  const Vec3f32 up(0.0f, 1.0f, 0.0f);
  mScene->set_camera(Camera(cameraPos, lookAt, up));

  struct Vertex final {
    f32 mX, mY, mZ;
    u32 mColor;
  };

  array<Vertex, 3u> vertices{{
    {-1.0f, -1.0f, 0.0f, Color(255, 0, 0).to_argb()}
  , {1.0f, -1.0f, 0.0f, Color(0, 0, 255).to_argb()}
  , {0.0f, 1.0f, 0.0f, Color(255, 255, 255).to_argb()}
  }};

  const VertexLayout vertexLayout = {
    {VertexElementType::F32_3, VertexElementUsage::Position}
  , {VertexElementType::U32_1, VertexElementUsage::ColorDiffuse}
  };

  auto& entityRegistry = mScene->get_entity_registry();
  auto entity = entityRegistry.create<TransformComponent, RenderComponent>();

  mTriangleEntity = std::get<0>(entity);

  auto& renderComponent = std::get<2>(entity);
  renderComponent.mMesh =
    basalt::get_renderer()->add_mesh(vertices.data(),
                                     static_cast<i32>(vertices.size()),
                                     vertexLayout, PrimitiveType::TriangleList);
  renderComponent.mRenderFlags = RenderFlagCullNone | RenderFlagDisableLighting;
}

void Matrices::on_show() {
  set_current_scene(mScene);
}

void Matrices::on_hide() {}

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
