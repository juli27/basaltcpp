#include "Textures.h"

#include <cmath> // cosf, sinf

#include <array>
#include <memory> // make_shared
#include <tuple>

#include <BasaltPrelude.h> // GetDeltaTime, SetCurrentScene, gfx

using std::array;

using bs::Color;
using bs::Scene;
using bs::TransformComponent;
using bs::math::PI;
using bs::math::Vec2f32;
using bs::math::Vec3f32;
using bs::gfx::Camera;
using bs::gfx::RenderComponent;
using bs::gfx::backend::PrimitiveType;
using bs::gfx::backend::RF_CULL_NONE;
using bs::gfx::backend::RF_DISABLE_LIGHTING;
using bs::gfx::backend::VertexElementType;
using bs::gfx::backend::VertexElementUsage;
using bs::gfx::backend::VertexLayout;

namespace d3d9_tuts {

Textures::Textures() : mScene(std::make_shared<Scene>()) {
  mScene->set_background_color(Color(0, 0, 255));
  mScene->set_camera(Camera(
    {0.0f, 3.0f, -5.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}
  ));

  struct Vertex final {
    Vec3f32 pos;
    u32 color = bs::Color().ToARGB();
    Vec2f32 uv;
  };

  array<Vertex, 50u * 2> vertices;

  for(uSize i = 0u; i < 50u; i++) {
    const auto theta = ( 2.0f * PI * i ) / ( 50 - 1 );
    const auto sinTheta = std::sinf(theta);
    const auto cosTheta = std::cosf(theta);

    vertices[2 * i].pos = {sinTheta, -1.0f, cosTheta};
    vertices[2 * i].color = Color(255, 255, 255).ToARGB();
    vertices[2 * i].uv = {i / (50.0f - 1), 1.0f};

    vertices[2 * i + 1].pos = {sinTheta, 1.0f, cosTheta};
    vertices[2 * i + 1].color = Color(128, 128, 128).ToARGB();
    vertices[2 * i + 1].uv = {i / (50.0f - 1), 0.0f};
  }

  const VertexLayout vertexLayout{
    { VertexElementUsage::POSITION, VertexElementType::F32_3 },
    { VertexElementUsage::COLOR_DIFFUSE, VertexElementType::U32_1 },
    { VertexElementUsage::TEXTURE_COORDS, VertexElementType::F32_2 }
  };

  auto& entityRegistry = mScene->get_entity_registry();
  auto entity = entityRegistry.create<TransformComponent, RenderComponent>();
  mCylinderEntity = std::get<0>(entity);

  auto& renderComponent = std::get<2>(entity);
  auto* renderer = bs::get_renderer();
  renderComponent.mMesh = renderer->AddMesh(
    vertices.data(), static_cast<i32>(vertices.size()), vertexLayout,
    PrimitiveType::TRIANGLE_STRIP
  );
  renderComponent.mTexture = renderer->AddTexture("data/banana.bmp");
  renderComponent.mRenderFlags = RF_CULL_NONE | RF_DISABLE_LIGHTING;
}

void Textures::OnShow() {
  bs::set_current_scene(mScene);
}

void Textures::OnHide() {}

void Textures::OnUpdate() {
  const auto deltaTime = static_cast<f32>(bs::get_delta_time());
  const auto radOffetX = PI * 0.5f * deltaTime;
  auto& transform =
    mScene->get_entity_registry().get<TransformComponent>(mCylinderEntity);
  transform.rotate(radOffetX, 0.0f, 0.0f);

  mScene->display_entity_gui(mCylinderEntity);
}

} // namespace d3d9_tuts
