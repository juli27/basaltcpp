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
using basalt::Scene;
using basalt::SceneView;
using basalt::System;
using basalt::Transform;
using basalt::Vector3f32;
using basalt::ViewPtr;
using basalt::gfx::Camera;
using basalt::gfx::Environment;
using basalt::gfx::MaterialCreateInfo;
using basalt::gfx::Model;
using basalt::gfx::PipelineCreateInfo;
using basalt::gfx::PrimitiveType;
using basalt::gfx::VertexElement;

namespace {

struct Vertex {
  Vector3f32 position;
  ColorEncoding::A8R8G8B8 color;

  static constexpr auto sLayout =
    basalt::gfx::make_vertex_layout<VertexElement::Position3F32,
                                    VertexElement::ColorDiffuse1U32A8R8G8B8>();
};

struct RotationSpeed {
  f32 degPerSecond;
};

class RotationSystem final : public System {
public:
  using UpdateBefore = basalt::TransformSystem;

  RotationSystem() noexcept = default;

  auto on_update(UpdateContext const& ctx) -> void override {
    auto const dt{ctx.deltaTime.count()};

    ctx.scene.entity_registry().view<Transform, RotationSpeed const>().each(
      [&](Transform& t, RotationSpeed const& rotationSpeed) {
        t.rotate_y(Angle::degrees(rotationSpeed.degPerSecond * dt));
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

  auto const material = [&] {
    auto materialInfo = MaterialCreateInfo{};

    auto& pipelineInfo = materialInfo.pipelineInfo;
    pipelineInfo.vertexLayout = Vertex::sLayout;
    pipelineInfo.primitiveType = PrimitiveType::TriangleList;

    materialInfo.pipeline = gfxCache->create_pipeline(pipelineInfo);

    return gfxCache->create_material(materialInfo);
  }();

  auto scene = Scene::create();
  scene->create_system<RotationSystem>();

  auto& gfxEnv = scene->entity_registry().ctx().emplace<Environment>();
  gfxEnv.set_background(Color::from_non_linear_rgba8(32, 32, 32));

  auto const triangle = scene->create_entity("Triangle"s);
  triangle.emplace<RotationSpeed>(360.0f);
  triangle.emplace<Model>(mesh, material);

  auto const cameraId = [&] {
    auto const camera =
      scene->create_entity("Camera"s, Vector3f32{0.0f, 3.0f, -5.0f});
    camera.emplace<Camera>(Vector3f32::zero(), Vector3f32::up(), 45_deg, 0.1f,
                           100.0f);

    return camera.entity();
  }();

  return SceneView::create(std::move(scene), std::move(gfxCache), cameraId);
}
