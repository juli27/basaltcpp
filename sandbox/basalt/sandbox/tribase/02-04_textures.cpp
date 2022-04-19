#include <basalt/sandbox/tribase/02-04_textures.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>
#include <basalt/api/scene_view.h>

#include <basalt/api/gfx/camera.h>
#include <basalt/api/gfx/resource_cache.h>

#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/scene/scene.h>
#include <basalt/api/scene/transform.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/vector3.h>

#include <entt/entity/handle.hpp>
#include <gsl/span>
#include <imgui/imgui.h>

#include <array>
#include <random>

using namespace entt::literals;

using std::array;

using gsl::span;

using namespace basalt::literals;

using basalt::Engine;
using basalt::InputEvent;
using basalt::InputEventHandled;
using basalt::Key;
using basalt::Scene;
using basalt::SceneView;
using basalt::Transform;
using basalt::Vector3f32;
using basalt::gfx::Camera;
using basalt::gfx::MaterialDescriptor;
using basalt::gfx::Mesh;
using basalt::gfx::MeshDescriptor;
using basalt::gfx::PrimitiveType;
using basalt::gfx::RenderComponent;
using basalt::gfx::Texture;
using basalt::gfx::TextureFilter;
using basalt::gfx::TextureMipFilter;
using basalt::gfx::VertexElement;
using basalt::gfx::VertexLayout;

namespace tribase {

namespace {

constexpr i32 NUM_TRIANGLES {1024};

struct TriangleData final {
  Vector3f32 velocity {0.0f};
  Vector3f32 rotationVelocity {0.0f};
};

struct RandomNumberGenerator final {
  auto generate_f32(const f32 min, const f32 max) -> f32 {
    return min +
           (max - min) * static_cast<f32>(mRandomEngine() % 10001) / 10000.0f;
  }

  auto generate_vector3f32() -> Vector3f32 {
    return Vector3f32::normalize(Vector3f32 {generate_f32(-1.0f, 1.0f),
                                             generate_f32(-1.0f, 1.0f),
                                             generate_f32(-1.0f, 1.0f)});
  }

private:
  std::default_random_engine mRandomEngine {std::random_device {}()};
};

auto create_camera() -> Camera {
  return Camera {Vector3f32 {0.0f},
                 Vector3f32::forward(),
                 Vector3f32::up(),
                 90.0_deg,
                 0.1f,
                 100.0f};
}

} // namespace

Textures::Textures(Engine& engine)
  : mScene {std::make_shared<Scene>()}
  , mSceneView {std::make_shared<SceneView>(mScene, create_camera())} {
  add_child_top(mSceneView);

  mScene->set_background(Color::from_non_linear_rgba8(0, 0, 63));

  const VertexLayout vertexLayout {VertexElement::Position3F32,
                                   VertexElement::ColorDiffuse1U32A8R8G8B8,
                                   VertexElement::TextureCoords2F32};

  const Texture texture {
    engine.get_or_load<Texture>("data/tribase/Texture.bmp"_hs)};

  MaterialDescriptor materialDesc {};
  materialDesc.sampledTexture.texture = texture;
  materialDesc.primitiveType = PrimitiveType::TriangleList;
  materialDesc.cullBackFace = false;
  materialDesc.lit = false;

  auto& gfxResources {engine.gfx_resource_cache()};
  mPointSampler = gfxResources.create_material(materialDesc);

  materialDesc.sampledTexture.filter = TextureFilter::Linear;
  materialDesc.sampledTexture.mipFilter = TextureMipFilter::Linear;
  mLinearSamplerWithMip = gfxResources.create_material(materialDesc);

  materialDesc.sampledTexture.filter = TextureFilter::LinearAnisotropic;
  materialDesc.sampledTexture.mipFilter = TextureMipFilter::None;
  mAnisotropicSampler = gfxResources.create_material(materialDesc);

  RandomNumberGenerator rng {};

  for (i32 i {0}; i < NUM_TRIANGLES; ++i) {
    const f32 scale {rng.generate_f32(1.0f, 5.0f)};
    auto triangle {mScene->create_entity(
      Vector3f32 {0.0f, 0.0f, 50.0f}, Vector3f32 {0.0f}, Vector3f32 {scale})};

    const Vector3f32 velocity {rng.generate_vector3f32() *
                               rng.generate_f32(0.1f, 5.0f)};
    const Vector3f32 rotationVelocity {rng.generate_f32(-1.0f, 1.0f),
                                       rng.generate_f32(-1.0f, 1.0f),
                                       rng.generate_f32(-1.0f, 1.0f)};
    triangle.emplace<TriangleData>(velocity, rotationVelocity);

    struct Vertex final {
      f32 x;
      f32 y;
      f32 z;
      ColorEncoding::A8R8G8B8 color;
      f32 u;
      f32 v;
    };

    array<Vertex, 3> vertices {};

    for (auto& vertex : vertices) {
      const auto pos {rng.generate_vector3f32()};
      vertex.x = pos.x();
      vertex.y = pos.y();
      vertex.z = pos.z();
      vertex.color = Color::from_non_linear(rng.generate_f32(0.0f, 1.0f),
                                            rng.generate_f32(0.0f, 1.0f),
                                            rng.generate_f32(0.0f, 1.0f))
                       .to_argb();
      vertex.u = rng.generate_f32(-1.0f, 2.0f);
      vertex.v = rng.generate_f32(-1.0f, 2.0f);
    }

    const MeshDescriptor meshDesc {
      as_bytes(span {vertices}),
      static_cast<u32>(vertices.size()),
      vertexLayout,
    };
    triangle.emplace<RenderComponent>(gfxResources.create_mesh(meshDesc),
                                      mPointSampler);
  }
}

void Textures::on_tick(Engine& engine) {
  mTimeAccum += engine.delta_time();

  if (!is_key_down(Key::Space)) {
    mScene->ecs().view<Transform, TriangleData>().each(
      [&](Transform& transform, TriangleData& data) {
        const f32 deltaTime {static_cast<f32>(engine.delta_time())};

        transform.position += data.velocity * deltaTime;
        transform.rotation += data.rotationVelocity * deltaTime;

        if (transform.position.length() > 100.0f) {
          data.velocity *= -1.0f;
        }
      });
  }

  const char* currentMode;

  if (static_cast<i32>(mTimeAccum / 3.0) % 3 == 0) {
    currentMode = "MIN: Linear, MAG: Linear, MIP: Linear";
    mScene->ecs().view<RenderComponent>().each(
      [this](RenderComponent& rc) { rc.material = mLinearSamplerWithMip; });
  } else if (static_cast<i32>(mTimeAccum / 3.0) % 3 == 2) {
    currentMode = "MIN: Point, MAG: Point, MIP: None";
    mScene->ecs().view<RenderComponent>().each(
      [this](RenderComponent& rc) { rc.material = mPointSampler; });
  } else {
    currentMode = "MIN: Anisotropic, MAG: Anisotropic, MIP: None";
    mScene->ecs().view<RenderComponent>().each(
      [this](RenderComponent& rc) { rc.material = mAnisotropicSampler; });
  }

  if (ImGui::Begin("Textures##TribaseTextures")) {
    ImGui::TextUnformatted("Hold SPACE to stop animating");
    ImGui::TextUnformatted(currentMode);
  }

  ImGui::End();
}

auto Textures::on_input(const InputEvent&) -> InputEventHandled {
  return InputEventHandled::Yes;
}

} // namespace tribase
