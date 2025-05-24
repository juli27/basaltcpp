#include "samples.h"

#include "basalt/sandbox/shared/debug_scene_view.h"

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

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
#include <basalt/api/math/matrix4x4.h>
#include <basalt/api/math/vector2.h>
#include <basalt/api/math/vector3.h>

#include <gsl/span>
#include <imgui.h>

#include <array>
#include <string>
#include <string_view>
#include <utility>

using namespace std::literals;
using std::array;

using gsl::span;

using namespace basalt::literals;

using basalt::Angle;
using basalt::Engine;
using basalt::Entity;
using basalt::EntityId;
using basalt::Matrix4x4f32;
using basalt::PI;
using basalt::Scene;
using basalt::System;
using basalt::Transform;
using basalt::Vector2f32;
using basalt::Vector3f32;
using basalt::ViewPtr;
using basalt::gfx::Camera;
using basalt::gfx::Environment;
using basalt::gfx::FixedFragmentShaderCreateInfo;
using basalt::gfx::FixedVertexShaderCreateInfo;
using basalt::gfx::MaterialColorSource;
using basalt::gfx::MaterialCreateInfo;
using basalt::gfx::MaterialHandle;
using basalt::gfx::Model;
using basalt::gfx::PipelineCreateInfo;
using basalt::gfx::PrimitiveType;
using basalt::gfx::TestPassCond;
using basalt::gfx::TextureCoordinateSet;
using basalt::gfx::TextureCoordinateSrc;
using basalt::gfx::TextureCoordinateTransformMode;
using basalt::gfx::TextureStage;
using basalt::gfx::VertexElement;

namespace {

constexpr auto sTextureFilePath = "data/banana.bmp"sv;

struct Vertex {
  Vector3f32 position;
  Vector3f32 normal;
  Vector2f32 texture;

  static constexpr auto sLayout =
    basalt::gfx::make_vertex_layout<VertexElement::Position3F32,
                                    VertexElement::Normal3F32,
                                    VertexElement::TextureCoords2F32>();
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

struct Settings {
  MaterialHandle material;
  MaterialHandle tciMaterial;
};

class SettingsSystem final : public System {
public:
  using UpdateBefore = basalt::gfx::GfxSystem;

  static constexpr auto sCylinder = entt::hashed_string::value("cylinder");

  auto on_update(UpdateContext const& ctx) -> void override {
    auto& ecs = ctx.scene.entity_registry();
    auto& sceneCtx = ecs.ctx();
    auto const& settings = sceneCtx.get<Settings>();
    auto const cylinder =
      ctx.scene.get_handle(sceneCtx.get<EntityId>(sCylinder));

    auto& material = cylinder.get<Model>().material;

    if (ImGui::Begin("Settings##SimpleScene")) {
      auto showTci = material == settings.tciMaterial;

      if (ImGui::Checkbox("Show TCI", &showTci)) {
        material = showTci ? settings.tciMaterial : settings.material;
      }
    }

    ImGui::End();
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
      vertices[2 * i] = Vertex{{y, -1.0f, x}, normal, {ratio, 1.0f}};
      vertices[2 * i + 1] = Vertex{{y, 1.0f, x}, normal, {ratio, 0.0f}};
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

    auto fs = FixedFragmentShaderCreateInfo{};
    constexpr auto textureStages = array{TextureStage{}};
    fs.textureStages = textureStages;

    auto& pipelineInfo = materialInfo.pipelineInfo;
    pipelineInfo.vertexShader = &vs;
    pipelineInfo.fragmentShader = &fs;
    pipelineInfo.vertexLayout = Vertex::sLayout;
    pipelineInfo.primitiveType = PrimitiveType::TriangleStrip;
    pipelineInfo.depthTest = TestPassCond::IfLessEqual;
    pipelineInfo.depthWriteEnable = true;

    materialInfo.pipeline = gfxCache->create_pipeline(pipelineInfo);
    materialInfo.diffuse = Colors::WHITE;
    materialInfo.ambient = Colors::WHITE;
    materialInfo.sampledTexture.sampler = gfxCache->create_sampler({});
    materialInfo.sampledTexture.texture =
      gfxCache->load_texture_2d(sTextureFilePath);

    return gfxCache->create_material(materialInfo);
  }();

  auto const tciMaterial = [&] {
    auto materialInfo = MaterialCreateInfo{};

    auto vs = FixedVertexShaderCreateInfo{};
    auto texCoordinateSets = array{TextureCoordinateSet{}};
    auto& coordinateSet = std::get<0>(texCoordinateSets);
    coordinateSet.src = TextureCoordinateSrc::PositionInViewSpace;
    coordinateSet.transformMode = TextureCoordinateTransformMode::Count4;
    coordinateSet.projected = true;
    vs.textureCoordinateSets = texCoordinateSets;
    vs.lightingEnabled = true;
    vs.diffuseSource = MaterialColorSource::Material;

    auto fs = FixedFragmentShaderCreateInfo{};
    constexpr auto textureStages = array{TextureStage{}};
    fs.textureStages = textureStages;

    auto& pipelineInfo = materialInfo.pipelineInfo;
    pipelineInfo.vertexShader = &vs;
    pipelineInfo.fragmentShader = &fs;
    pipelineInfo.vertexLayout = Vertex::sLayout;
    pipelineInfo.primitiveType = PrimitiveType::TriangleStrip;
    pipelineInfo.depthTest = TestPassCond::IfLessEqual;
    pipelineInfo.depthWriteEnable = true;

    materialInfo.pipeline = gfxCache->create_pipeline(pipelineInfo);
    materialInfo.texTransform = Matrix4x4f32::scaling(0.5f, -0.5f, 1.0f) *
                                Matrix4x4f32::translation(0.5f, 0.5f, 0.0f);
    materialInfo.diffuse = Colors::WHITE;
    materialInfo.ambient = Colors::WHITE;
    materialInfo.sampledTexture.sampler = gfxCache->create_sampler({});
    materialInfo.sampledTexture.texture =
      gfxCache->load_texture_2d(sTextureFilePath);

    return gfxCache->create_material(materialInfo);
  }();

  auto scene = Scene::create();
  scene->create_system<RotationSystem>();
  scene->create_system<DirectionalLightSystem>();
  scene->create_system<SettingsSystem>();

  auto& sceneCtx = scene->entity_registry().ctx();
  auto& gfxEnv = sceneCtx.emplace<Environment>();
  gfxEnv.set_background(Color::from_non_linear_rgba8(32, 32, 32));
  gfxEnv.set_ambient_light(Color::from(0x00202020_a8r8g8b8));
  gfxEnv.add_directional_light(Vector3f32::normalized(1, 1, 0), Colors::WHITE,
                               {}, {});

  sceneCtx.insert_or_assign(Settings{material, tciMaterial});

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

  sceneCtx.emplace_as<EntityId>(SettingsSystem::sCylinder, cylinder.entity());

  return DebugSceneView::create(std::move(scene), std::move(gfxCache),
                                cameraId);
}
