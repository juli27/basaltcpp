#include <basalt/sandbox/samples/samples.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>
#include <basalt/api/scene_view.h>

#include <basalt/api/gfx/camera.h>
#include <basalt/api/gfx/environment.h>
#include <basalt/api/gfx/resource_cache.h>

#include <basalt/api/scene/ecs.h>
#include <basalt/api/scene/scene.h>
#include <basalt/api/scene/system.h>
#include <basalt/api/scene/transform.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/vector3.h>

#include <gsl/span>

#include <array>
#include <utility>

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
using basalt::ViewPtr;
using basalt::gfx::Camera;
using basalt::gfx::Environment;
using basalt::gfx::MaterialCreateInfo;
using basalt::gfx::PipelineCreateInfo;
using basalt::gfx::PrimitiveType;
using basalt::gfx::RenderComponent;
using basalt::gfx::TestPassCond;
using basalt::gfx::VertexElement;

namespace {

struct Vertex final {
  Vector3f32 pos{};
  ColorEncoding::A8R8G8B8 color{};

  static constexpr auto sLayout = array{
    VertexElement::Position3F32,
    VertexElement::ColorDiffuse1U32A8R8G8B8,
  };
};

struct RotationSpeed final {
  f32 rotationDegPerSecond;
};

class RotationSpeedSystem final : public System {
public:
  using UpdateBefore = basalt::TransformSystem;

  RotationSpeedSystem() noexcept = default;

  auto on_update(UpdateContext const& ctx) -> void override {
    auto const dt{ctx.deltaTime.count()};

    ctx.scene.entity_registry().view<Transform, RotationSpeed const>().each(
      [&](Transform& t, RotationSpeed const& rotationSpeed) {
        t.rotate_y(Angle::degrees(rotationSpeed.rotationDegPerSecond * dt));
      });
  }
};

} // namespace

auto Samples::new_simple_scene_sample(Engine& engine) -> ViewPtr {
  auto gfxCache = engine.create_gfx_resource_cache();

  constexpr auto vertices =
    array{Vertex{{-1.0f, -1.0f, 0.0f}, 0xffff0000_a8r8g8b8},
          Vertex{{1.0f, -1.0f, 0.0f}, 0xff0000ff_a8r8g8b8},
          Vertex{{0.0f, 1.0f, 0.0f}, 0xffffffff_a8r8g8b8}};

  auto const mesh = gfxCache->create_mesh({
    as_bytes(span{vertices}),
    static_cast<u32>(vertices.size()),
    Vertex::sLayout,
  });

  auto materialDesc = MaterialCreateInfo{};
  materialDesc.pipeline = [&] {
    auto pipelineDesc = PipelineCreateInfo{};
    pipelineDesc.vertexLayout = Vertex::sLayout;
    pipelineDesc.primitiveType = PrimitiveType::TriangleList;
    pipelineDesc.depthTest = TestPassCond::IfLessEqual;
    pipelineDesc.depthWriteEnable = true;

    return gfxCache->create_pipeline(pipelineDesc);
  }();
  auto const material = gfxCache->create_material(materialDesc);

  auto scene = Scene::create();
  auto& gfxEnv = scene->entity_registry().ctx().emplace<Environment>();
  gfxEnv.set_background(Color::from_non_linear(0.103f, 0.103f, 0.103f));

  auto const triangle = scene->create_entity(Vector3f32::forward() * 2.5f);
  triangle.emplace<RotationSpeed>(360.0f);
  triangle.emplace<RenderComponent>(mesh, material);

  scene->create_system<RotationSpeedSystem>();

  auto const cameraId = [&] {
    auto const camera = scene->create_entity();
    camera.emplace<Camera>(Vector3f32::forward(), Vector3f32::up(), 90_deg,
                           0.1f, 100.0f);

    return camera.entity();
  }();

  return SceneView::create(std::move(scene), std::move(gfxCache), cameraId);
}
