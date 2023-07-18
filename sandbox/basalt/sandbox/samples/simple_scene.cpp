#include <basalt/sandbox/samples/simple_scene.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>
#include <basalt/api/scene_view.h>

#include <basalt/api/gfx/camera.h>
#include <basalt/api/gfx/resource_cache.h>

#include <basalt/api/scene/ecs.h>
#include <basalt/api/scene/scene.h>
#include <basalt/api/scene/system.h>
#include <basalt/api/scene/transform.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/vector3.h>

#include <gsl/span>

#include <array>

namespace samples {

using std::array;

using gsl::span;

using namespace basalt::literals;

using basalt::Angle;
using basalt::Engine;
using basalt::Entity;
using basalt::Scene;
using basalt::SceneView;
using basalt::System;
using basalt::Transform;
using basalt::Vector3f32;
using basalt::gfx::Camera;
using basalt::gfx::MaterialDescriptor;
using basalt::gfx::PipelineDescriptor;
using basalt::gfx::PrimitiveType;
using basalt::gfx::RenderComponent;
using basalt::gfx::TestPassCond;
using basalt::gfx::VertexElement;

namespace {

struct Vertex final {
  array<f32, 3> pos {};
  ColorEncoding::A8R8G8B8 color {};

  static constexpr array sLayout {VertexElement::Position3F32,
                                  VertexElement::ColorDiffuse1U32A8R8G8B8};
};

struct RotationSpeed final {
  Angle rotationPerSecond;
};

class RotationSpeedSystem final : public System {
public:
  using UpdateBefore = basalt::TransformSystem;

  RotationSpeedSystem() noexcept = default;

  auto on_update(const UpdateContext& ctx) -> void override {
    const auto dt {ctx.deltaTime.count()};

    ctx.scene.entity_registry().view<Transform, const RotationSpeed>().each(
      [&](Transform& t, const RotationSpeed& rotationSpeed) {
        t.rotate_y(
          Angle::degrees(rotationSpeed.rotationPerSecond.degrees() * dt));
      });
  }
};

auto add_camera(Scene& scene) -> Entity {
  const Entity camera {scene.create_entity()};
  camera.emplace<Camera>(Vector3f32::forward(), Vector3f32::up(), 90_deg, 0.1f,
                         100.0f);

  return camera;
}

} // namespace

SimpleScene::SimpleScene(Engine& engine)
  : mGfxCache {engine.create_gfx_resource_cache()} {
  const array vertices {Vertex {{-1.0f, -1.0f, 0.0f}, 0xffff0000_a8r8g8b8},
                        Vertex {{1.0f, -1.0f, 0.0f}, 0xff0000ff_a8r8g8b8},
                        Vertex {{0.0f, 1.0f, 0.0f}, 0xffffffff_a8r8g8b8}};

  mMesh = mGfxCache->create_mesh({
    as_bytes(span {vertices}),
    static_cast<u32>(vertices.size()),
    Vertex::sLayout,
  });

  PipelineDescriptor pipelineDesc;
  pipelineDesc.vertexInputState = Vertex::sLayout;
  pipelineDesc.primitiveType = PrimitiveType::TriangleList;
  pipelineDesc.lightingEnabled = false;
  pipelineDesc.depthTest = TestPassCond::IfLessEqual;
  pipelineDesc.depthWriteEnable = true;
  MaterialDescriptor materialDesc;
  materialDesc.pipelineDesc = &pipelineDesc;
  mMaterial = mGfxCache->create_material(materialDesc);

  const auto scene {Scene::create()};
  scene->set_background(Color::from_non_linear(0.103f, 0.103f, 0.103f));

  const Entity triangle {scene->create_entity(Vector3f32::forward() * 2.5f)};
  triangle.emplace<RotationSpeed>(360_deg);
  triangle.emplace<RenderComponent>(mMesh, mMaterial);

  const Entity camera {add_camera(*scene)};

  scene->create_system<RotationSpeedSystem>();
  add_child_top(SceneView::create(scene, mGfxCache, camera.entity()));
}

} // namespace samples
