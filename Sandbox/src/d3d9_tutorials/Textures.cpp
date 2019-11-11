#include "Textures.h"

#include <basalt/Prelude.h> // GetDeltaTime, SetCurrentScene, gfx

#include <array>
#include <memory> // make_shared
#include <tuple>

#include <cmath> // cosf, sinf

using std::array;

using basalt::Color;
using basalt::Scene;
using basalt::TransformComponent;
using basalt::math::PI;
using basalt::math::Vec2f32;
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

Textures::Textures() : mScene(std::make_shared<Scene>()) {
  mScene->set_background_color(Color(0, 0, 255));
  mScene->set_camera(Camera(
    {0.0f, 3.0f, -5.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}
  ));

  struct Vertex final {
    Vec3f32 pos;
    u32 color = Color().to_argb();
    Vec2f32 uv;
  };

  array<Vertex, 50u * 2> vertices;

  for(uSize i = 0u; i < 50u; i++) {
    const auto theta = ( 2.0f * PI * i ) / ( 50 - 1 );
    const auto sinTheta = std::sinf(theta);
    const auto cosTheta = std::cosf(theta);

    vertices[2 * i].pos = {sinTheta, -1.0f, cosTheta};
    vertices[2 * i].color = Color(255, 255, 255).to_argb();
    vertices[2 * i].uv = {i / (50.0f - 1), 1.0f};

    vertices[2 * i + 1].pos = {sinTheta, 1.0f, cosTheta};
    vertices[2 * i + 1].color = Color(128, 128, 128).to_argb();
    vertices[2 * i + 1].uv = {i / (50.0f - 1), 0.0f};
  }

  const VertexLayout vertexLayout{
    { VertexElementUsage::Position, VertexElementType::F32_3 },
    { VertexElementUsage::ColorDiffuse, VertexElementType::U32_1 },
    { VertexElementUsage::TextureCoords, VertexElementType::F32_2 }
  };

  auto& entityRegistry = mScene->get_entity_registry();
  auto entity = entityRegistry.create<TransformComponent, RenderComponent>();
  mCylinderEntity = std::get<0>(entity);

  auto& renderComponent = std::get<2>(entity);
  auto* renderer = basalt::get_renderer();
  renderComponent.mMesh = renderer->add_mesh(
    vertices.data(), static_cast<i32>(vertices.size()), vertexLayout,
    PrimitiveType::TriangleStrip
  );
  renderComponent.mTexture = renderer->add_texture("data/banana.bmp");
  renderComponent.mRenderFlags = RenderFlagCullNone | RenderFlagDisableLighting;
}

void Textures::on_show() {
  set_current_scene(mScene);
}

void Textures::on_hide() {}

void Textures::on_update() {
  const auto deltaTime = static_cast<f32>(basalt::get_delta_time());
  const auto radOffetX = PI * 0.5f * deltaTime;
  auto& transform =
    mScene->get_entity_registry().get<TransformComponent>(mCylinderEntity);
  transform.rotate(radOffetX, 0.0f, 0.0f);

  mScene->display_entity_gui(mCylinderEntity);
}

} // namespace d3d9_tuts
