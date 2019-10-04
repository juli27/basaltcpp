#include "Matrices.h"

#include <basalt/Prelude.h> // GetDeltaTime, SetCurrentScene, gfx, ImGui

#include <array>
#include <memory> // make_shared
#include <tuple> // get

using std::array;

using bs::Color;
using bs::Scene;
using bs::TransformComponent;
using bs::math::Vec3f32;
using bs::gfx::Camera;
using bs::gfx::RenderComponent;
using bs::gfx::backend::PrimitiveType;
using bs::gfx::backend::RenderFlagCullNone;
using bs::gfx::backend::RenderFlagDisableLighting;
using bs::gfx::backend::VertexElementType;
using bs::gfx::backend::VertexElementUsage;
using bs::gfx::backend::VertexLayout;

namespace d3d9_tuts {

Matrices::Matrices() : mScene(std::make_shared<Scene>()) {
  mScene->set_background_color(Color(0, 0, 0));

  const Vec3f32 cameraPos(0.0f, 3.0f, -5.0f);
  const Vec3f32 lookAt(0.0f, 0.0f, 0.0f);
  const Vec3f32 up(0.0f, 1.0f, 0.0f);
  mScene->set_camera(Camera(cameraPos, lookAt, up));

  struct Vertex final {
    f32 x, y, z;
    u32 color;
  };

  array<Vertex, 3u> vertices{{
    {-1.0f, -1.0f, 0.0f, Color(255, 0  , 0  ).to_argb()},
    { 1.0f, -1.0f, 0.0f, Color(0  , 0  , 255).to_argb()},
    { 0.0f,  1.0f, 0.0f, Color(255, 255, 255).to_argb()}
  }};

  const VertexLayout vertexLayout = {
    { VertexElementUsage::Position, VertexElementType::F32_3 },
    { VertexElementUsage::ColorDiffuse, VertexElementType::U32_1 }
  };

  auto& entityRegistry = mScene->get_entity_registry();
  auto entity = entityRegistry.create<TransformComponent, RenderComponent>();
  mTriangleEntity = std::get<0>(entity);
  auto& renderComponent = std::get<2>(entity);
  renderComponent.mMesh = bs::get_renderer()->add_mesh(
    vertices.data(), static_cast<i32>(vertices.size()), vertexLayout,
    PrimitiveType::TriangleList
  );
  renderComponent.mRenderFlags = RenderFlagCullNone | RenderFlagDisableLighting;
}

void Matrices::on_show() {
  bs::set_current_scene(mScene);
}

void Matrices::on_hide() {}

void Matrices::on_update() {
  const auto deltaTime = static_cast<f32>(bs::get_delta_time());

  static auto speed = 2.0f;
  const auto radOffsetY = deltaTime * speed * bs::math::PI;
  auto& transform =
    mScene->get_entity_registry().get<TransformComponent>(mTriangleEntity);
  transform.rotate(0.0f, radOffsetY, 0.0f);

  ImGui::Begin("D3D9 Tutorial 03: Matrices");
  ImGui::SliderFloat("Rotation Speed", &speed, -2.0f, 2.0f);
  ImGui::End();
}

} // namespace d3d9_tuts
