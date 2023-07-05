#include <basalt/sandbox/samples/simple_scene.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>
#include <basalt/api/scene_view.h>

#include <basalt/api/gfx/camera.h>
#include <basalt/api/gfx/resource_cache.h>

#include <basalt/api/scene/scene.h>
#include <basalt/api/scene/system.h>
#include <basalt/api/scene/transform.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/vector3.h>

#include <entt/entity/handle.hpp>

#include <gsl/span>

#include <array>

namespace samples {

using std::array;

using entt::handle;
using gsl::span;

using namespace basalt::literals;

using basalt::Angle;
using basalt::Engine;
using basalt::Scene;
using basalt::ScenePtr;
using basalt::SceneView;
using basalt::System;
using basalt::SystemContext;
using basalt::Transform;
using basalt::Vector3f32;
using basalt::gfx::Camera;
using basalt::gfx::MaterialDescriptor;
using basalt::gfx::MeshDescriptor;
using basalt::gfx::PrimitiveType;
using basalt::gfx::RenderComponent;
using basalt::gfx::VertexElement;

namespace {

struct RotationSpeed final {
  Angle rotationPerSecond;
};

class RotationSpeedSystem final : public System {
public:
  RotationSpeedSystem() noexcept = default;

  auto on_update(const SystemContext& ctx) -> void override {
    const auto dt {static_cast<f32>(ctx.deltaTimeSeconds)};

    ctx.scene.ecs().view<Transform, const RotationSpeed>().each(
      [&](Transform& t, const RotationSpeed& rotationSpeed) {
        t.rotate(0.0_rad,
                 Angle::degrees(rotationSpeed.rotationPerSecond.degrees() * dt),
                 0.0_rad);
      });
  }
};

auto create_camera() -> Camera {
  return Camera {
    Vector3f32 {0.0f},
    Vector3f32::forward(),
    Vector3f32::up(),
    90.0_deg,
    0.1f,
    100.0f,
  };
}

} // namespace

SimpleScene::SimpleScene(Engine& engine)
  : mGfxResources {engine.gfx_resource_cache()} {
  const ScenePtr scene {Scene::create()};
  scene->create_system<RotationSpeedSystem>();

  add_child_top(SceneView::create(scene, create_camera()));

  scene->set_background(Color::from_non_linear(0.103f, 0.103f, 0.103f));

  const handle triangle {scene->create_entity(Vector3f32::forward() * 2.5f)};

  triangle.emplace<RotationSpeed>(360_deg);

  struct Vertex final {
    f32 x;
    f32 y;
    f32 z;
    ColorEncoding::A8R8G8B8 color;
  };

  const array vertices {
    Vertex {-1.0f, -1.0f, 0.0f, ColorEncoding::pack_a8r8g8b8_u32(255, 0, 0)},
    Vertex {1.0f, -1.0f, 0.0f, ColorEncoding::pack_a8r8g8b8_u32(0, 0, 255)},
    Vertex {0.0f, 1.0f, 0.0f, ColorEncoding::pack_a8r8g8b8_u32(255, 255, 255)},
  };

  const array vertexLayout {
    VertexElement::Position3F32,
    VertexElement::ColorDiffuse1U32A8R8G8B8,
  };

  mMesh = mGfxResources.create_mesh(MeshDescriptor {
    as_bytes(span {vertices}),
    static_cast<u32>(vertices.size()),
    vertexLayout,
  });

  MaterialDescriptor materialDescriptor {};
  materialDescriptor.vertexInputState = vertexLayout;
  materialDescriptor.primitiveType = PrimitiveType::TriangleList;
  materialDescriptor.lit = false;
  materialDescriptor.cullBackFace = false;
  mMaterial = mGfxResources.create_material(materialDescriptor);

  auto& rc {triangle.emplace<RenderComponent>()};
  rc.mesh = mMesh;
  rc.material = mMaterial;
}

SimpleScene::~SimpleScene() noexcept {
  mGfxResources.destroy(mMaterial);
  mGfxResources.destroy(mMesh);
}

} // namespace samples
