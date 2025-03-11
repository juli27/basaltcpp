#include <basalt/sandbox/samples/samples.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>
#include <basalt/api/scene_view.h>

#include <basalt/api/gfx/camera.h>
#include <basalt/api/gfx/environment.h>
#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/backend/vertex_layout.h>

#include <basalt/api/scene/ecs.h>
#include <basalt/api/scene/scene.h>
#include <basalt/api/scene/system.h>
#include <basalt/api/scene/transform.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/constants.h>
#include <basalt/api/math/vector2.h>
#include <basalt/api/math/vector3.h>

#include <gsl/span>

#include <array>
#include <string>
#include <utility>

using namespace std::literals;
using std::array;

using gsl::span;

using namespace basalt::literals;

using basalt::Angle;
using basalt::Engine;
using basalt::Entity;
using basalt::PI;
using basalt::Scene;
using basalt::SceneView;
using basalt::System;
using basalt::Transform;
using basalt::Vector2f32;
using basalt::Vector3f32;
using basalt::ViewPtr;
using basalt::gfx::Camera;
using basalt::gfx::Environment;
using basalt::gfx::FixedVertexShaderCreateInfo;
using basalt::gfx::MaterialColorSource;
using basalt::gfx::MaterialCreateInfo;
using basalt::gfx::Model;
using basalt::gfx::PipelineCreateInfo;
using basalt::gfx::PrimitiveType;
using basalt::gfx::TestPassCond;
using basalt::gfx::VertexElement;

namespace {

struct Vertex {
  Vector3f32 position;
  Vector3f32 normal;

  static constexpr auto sLayout =
    basalt::gfx::make_vertex_layout<VertexElement::Position3F32,
                                    VertexElement::Normal3F32>();
};

struct RotationSpeed {
  f32 radPerSecond;
};

class RotationSystem final : public System {
public:
  using UpdateBefore = basalt::TransformSystem;

  RotationSystem() noexcept = default;

  auto on_update(UpdateContext const& ctx) -> void override {
    auto const dt = ctx.deltaTime.count();

    ctx.scene.entity_registry().view<Transform, RotationSpeed const>().each(
      [&](Transform& t, RotationSpeed const& rotationSpeed) {
        t.rotate_x(Angle::radians(rotationSpeed.radPerSecond * dt));
      });
  }
};

class DirectionalLightSystem final : public System {
public:
  using UpdateBefore = basalt::gfx::GfxSystem;

  DirectionalLightSystem() noexcept = default;

  auto on_update(UpdateContext const& ctx) -> void override {
    auto const dt = ctx.deltaTime.count();

    // 1/350 rad/ms = 20/7 rad/s
    auto constexpr radPerSecond = 20.0f / 7.0f;

    auto& env = ctx.scene.entity_registry().ctx().get<Environment>();
    auto light = env.directional_lights().front();

    auto const dirXZPlane =
      Vector2f32{light.directionInWorld.x(), light.directionInWorld.z()};
    auto rotationY = Angle::radians(std::atan2(dirXZPlane.y(), dirXZPlane.x()));

    rotationY += Angle::radians(radPerSecond * dt);
    light.directionInWorld =
      Vector3f32::normalized(rotationY.cos(), 1.0f, rotationY.sin());

    env.clear_directional_lights();
    env.add_directional_light(light);
  }
};

} // namespace

auto Samples::new_simple_scene_sample(Engine& engine) -> ViewPtr {
  auto gfxCache = engine.create_gfx_resource_cache();

  auto const mesh = [&] {
    constexpr auto vertexCount = u32{2 * 50};

    auto vertices = array<Vertex, vertexCount>{};
    // sample the unit circle in 49 different spots (0°/360° twice)
    for (auto i = u32{0}; i < 50; i++) {
      auto const ratio = static_cast<f32>(i) / (50.0f - 1.0f);
      // angle / length of arc of unit circle
      auto const theta = Angle::radians(2.0f * PI * ratio);

      auto const x = theta.cos();
      auto const y = theta.sin();

      auto const normal = Vector3f32{y, 0.0f, x};
      vertices[2 * i] = Vertex{{y, -1.0f, x}, normal};
      vertices[2 * i + 1] = Vertex{{y, 1.0f, x}, normal};
    }

    return gfxCache->create_mesh({as_bytes(span{vertices}),
                                  static_cast<u32>(vertices.size()),
                                  Vertex::sLayout});
  }();

  auto const material = [&] {
    auto materialInfo = MaterialCreateInfo{};

    auto vs = FixedVertexShaderCreateInfo{};
    vs.lightingEnabled = true;
    vs.diffuseSource = MaterialColorSource::Material;

    auto& pipelineInfo = materialInfo.pipelineInfo;
    pipelineInfo.vertexShader = &vs;
    pipelineInfo.vertexLayout = Vertex::sLayout;
    pipelineInfo.primitiveType = PrimitiveType::TriangleStrip;
    pipelineInfo.depthTest = TestPassCond::IfLessEqual;
    pipelineInfo.depthWriteEnable = true;

    materialInfo.pipeline = gfxCache->create_pipeline(pipelineInfo);
    materialInfo.diffuse = Colors::YELLOW;
    materialInfo.ambient = Colors::YELLOW;

    return gfxCache->create_material(materialInfo);
  }();

  auto scene = Scene::create();
  scene->create_system<RotationSystem>();
  scene->create_system<DirectionalLightSystem>();

  auto& gfxEnv = scene->entity_registry().ctx().emplace<Environment>();
  gfxEnv.set_background(Color::from_non_linear_rgba8(32, 32, 32));
  gfxEnv.set_ambient_light(Color::from(0x00202020_a8r8g8b8));
  gfxEnv.add_directional_light(Vector3f32::normalized(1, 1, 0), Colors::WHITE,
                               {}, {});

  auto const cylinder = scene->create_entity("Cylinder"s);
  // 1/500 rad/ms = 2 rad/s
  cylinder.emplace<RotationSpeed>(2.0f);
  cylinder.emplace<Model>(mesh, material);

  auto const cameraId = [&] {
    auto const camera =
      scene->create_entity("Camera"s, Vector3f32{0.0f, 3.0f, -5.0f});
    camera.emplace<Camera>(Vector3f32::zero(), Vector3f32::up(), 45_deg, 1.0f,
                           100.0f);

    return camera.entity();
  }();

  return SceneView::create(std::move(scene), std::move(gfxCache), cameraId);
}
