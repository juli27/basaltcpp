#include "samples.h"

#include <basalt/sandbox/shared/debug_scene_view.h>
#include <basalt/sandbox/shared/rotation_system.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/camera.h>
#include <basalt/api/gfx/context.h>
#include <basalt/api/gfx/environment.h>
#include <basalt/api/gfx/gfx_system.h>
#include <basalt/api/gfx/material.h>
#include <basalt/api/gfx/material_class.h>
#include <basalt/api/gfx/mesh.h>
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
using namespace basalt;

namespace {

constexpr auto sTextureFilePath = "data/banana.bmp"sv;

struct Vertex {
  Vector3f32 position;
  Vector3f32 normal;
  Vector2f32 texture;

  static constexpr auto sLayout =
    basalt::gfx::make_vertex_layout<gfx::VertexElement::Position3F32,
                                    gfx::VertexElement::Normal3F32,
                                    gfx::VertexElement::TextureCoords2F32>();
};

class DirectionalLightSystem final : public System {
public:
  using UpdateBefore = gfx::GfxSystem;

  auto on_update(UpdateContext const& ctx) -> void override {
    auto const dt = ctx.deltaTime.count();

    // 1/350 rad/ms = 20/7 rad/s
    auto constexpr radPerSecond = 20.0f / 7.0f;

    auto& env = ctx.scene.entity_registry().ctx().get<gfx::Environment>();
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

struct HasTextureTransform {};

class TextureTransformSystem final : public System {
public:
  using UpdateBefore = gfx::GfxSystem;

  auto on_update(UpdateContext const& ctx) -> void override {
    auto& scene = ctx.scene;
    auto& sceneCtx = scene.entity_registry().ctx();

    auto const cameraEntityId =
      sceneCtx.get<EntityId>(gfx::GfxSystem::sMainCamera);
    auto const cameraEntity =
      gfx::CameraEntity{scene.get_handle(cameraEntityId)};

    auto& gfxCtx = sceneCtx.get<gfx::Context&>();
    scene.entity_registry()
      .view<HasTextureTransform const, gfx::Model const>()
      .each([&](gfx::Model const& model) {
        auto& material = gfxCtx.get(model.material);
        material.set_value(gfx::MaterialPropertyId::TexTransform,
                           cameraEntity.view_to_clip() *
                             Matrix4x4f32::scaling(0.5f, -0.5f, 1.0f) *
                             Matrix4x4f32::translation(0.5f, 0.5f, 0.0f));
      });
  }
};

class SimpleSceneView : public View {
public:
  SimpleSceneView(SceneViewPtr sceneView, EntityId const cylinder,
                  gfx::MaterialHandle const material,
                  gfx::MaterialHandle const tciMaterial)
    : mScene{sceneView->scene()}
    , mCylinder{cylinder}
    , mMaterial{material}
    , mTciMaterial{tciMaterial} {
    add_child_bottom(std::move(sceneView));
  }

  auto on_update(UpdateContext&) -> void override {
    auto const cylinder = mScene->get_handle(mCylinder);

    if (ImGui::Begin("Settings##SimpleScene")) {
      auto showTci = cylinder.get<gfx::Model const>().material == mTciMaterial;

      if (ImGui::Checkbox("Show TCI", &showTci)) {
        if (showTci) {
          cylinder.patch<gfx::Model>(
            [&](gfx::Model& model) { model.material = mTciMaterial; });
          cylinder.emplace<HasTextureTransform>();
        } else {
          cylinder.patch<gfx::Model>(
            [&](gfx::Model& model) { model.material = mMaterial; });
          cylinder.erase<HasTextureTransform>();
        }
      }
    }

    ImGui::End();
  }

private:
  ScenePtr mScene;
  EntityId mCylinder;
  gfx::MaterialHandle mMaterial;
  gfx::MaterialHandle mTciMaterial;
};

} // namespace

auto Samples::new_simple_scene_sample(Engine& engine) -> ViewPtr {
  auto gfxCache = engine.create_gfx_resource_cache();

  auto const mesh = [&] {
    constexpr auto vertexCount = u32{2 * 50};

    auto vertices = std::array<Vertex, vertexCount>{};
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

    auto const vertexData = as_bytes(gsl::span{vertices});

    auto const vertexBuffer = gfxCache->create_vertex_buffer(
      gfx::VertexBufferCreateInfo{vertexData.size_bytes(), Vertex::sLayout},
      vertexData);

    return gfxCache->create_mesh(
      gfx::MeshCreateInfo{vertexBuffer, vertexCount});
  }();

  auto const material = [&] {
    auto info = gfx::MaterialCreateInfo{};
    info.clazz = [&] {
      auto info = gfx::MaterialClassCreateInfo{};
      auto& pipelineInfo = info.pipelineInfo;

      auto vs = gfx::FixedVertexShaderCreateInfo{};
      vs.lightingEnabled = true;
      vs.diffuseSource = gfx::MaterialColorSource::Material;
      pipelineInfo.vertexShader = &vs;

      auto fs = gfx::FixedFragmentShaderCreateInfo{};
      constexpr auto textureStages = std::array{gfx::TextureStage{}};
      fs.textureStages = textureStages;
      pipelineInfo.fragmentShader = &fs;

      pipelineInfo.vertexLayout = Vertex::sLayout;
      pipelineInfo.primitiveType = gfx::PrimitiveType::TriangleStrip;
      pipelineInfo.depthTest = gfx::TestPassCond::IfLessEqual;
      pipelineInfo.depthWriteEnable = true;

      return gfxCache->create_material_class(info);
    }();
    auto const properties = std::array{
      gfx::MaterialProperty{
        gfx::MaterialPropertyId::UniformColors,
        gfx::UniformColors{Colors::WHITE, Colors::WHITE},
      },
      gfx::MaterialProperty{
        gfx::MaterialPropertyId::SampledTexture,
        gfx::SampledTexture{gfxCache->create_sampler({}),
                            gfxCache->load_texture_2d(sTextureFilePath)}},
    };
    info.initialValues = properties;

    return gfxCache->create_material(info);
  }();

  auto const tciMaterial = [&] {
    auto info = gfx::MaterialCreateInfo{};
    info.clazz = [&] {
      auto info = gfx::MaterialClassCreateInfo{};
      auto& pipelineInfo = info.pipelineInfo;

      auto vs = gfx::FixedVertexShaderCreateInfo{};
      auto texCoordinateSets = std::array{gfx::TextureCoordinateSet{}};
      auto& coordinateSet = std::get<0>(texCoordinateSets);
      coordinateSet.src = gfx::TextureCoordinateSrc::PositionInViewSpace;
      coordinateSet.transformMode = gfx::TextureCoordinateTransformMode::Count4;
      coordinateSet.projected = true;
      vs.textureCoordinateSets = texCoordinateSets;
      vs.lightingEnabled = true;
      vs.diffuseSource = gfx::MaterialColorSource::Material;
      pipelineInfo.vertexShader = &vs;

      auto fs = gfx::FixedFragmentShaderCreateInfo{};
      constexpr auto textureStages = std::array{gfx::TextureStage{}};
      fs.textureStages = textureStages;
      pipelineInfo.fragmentShader = &fs;

      pipelineInfo.vertexLayout = Vertex::sLayout;
      pipelineInfo.primitiveType = gfx::PrimitiveType::TriangleStrip;
      pipelineInfo.depthTest = gfx::TestPassCond::IfLessEqual;
      pipelineInfo.depthWriteEnable = true;

      return gfxCache->create_material_class(info);
    }();
    auto const properties = std::array{
      gfx::MaterialProperty{
        gfx::MaterialPropertyId::UniformColors,
        gfx::UniformColors{Colors::WHITE, Colors::WHITE},
      },
      gfx::MaterialProperty{
        gfx::MaterialPropertyId::SampledTexture,
        gfx::SampledTexture{gfxCache->create_sampler({}),
                            gfxCache->load_texture_2d(sTextureFilePath)}},
      gfx::MaterialProperty{
        gfx::MaterialPropertyId::TexTransform,
        Matrix4x4f32::identity(),
      },
    };
    info.initialValues = properties;

    return gfxCache->create_material(info);
  }();

  auto scene = Scene::create();
  scene->create_system<RotationSystem>();
  scene->create_system<DirectionalLightSystem>();
  scene->create_system<TextureTransformSystem>();

  auto& sceneCtx = scene->entity_registry().ctx();
  auto& gfxEnv = sceneCtx.emplace<gfx::Environment>();
  gfxEnv.set_background(Color::from_non_linear_rgba8(32, 32, 32));
  gfxEnv.set_ambient_light(Color::from(0x00202020_a8r8g8b8));
  gfxEnv.add_directional_light(Vector3f32::normalized(1, 1, 0), Colors::WHITE,
                               {}, {});

  auto const cylinder = scene->create_entity("Cylinder"s);
  // 1/500 rad/ms = 2 rad/s
  cylinder.emplace<RotationSpeed>(2.0f, 0.0f, 0.0f);
  cylinder.emplace<gfx::Model>(mesh, material);

  auto const camera =
    scene->create_entity("Camera"s, Vector3f32{0.0f, 3.0f, -5.0f});
  camera.emplace<gfx::Camera>(Vector3f32::zero(), Vector3f32::up(), 45_deg,
                              1.0f, 100.0f);

  auto sceneView = DebugSceneView::create(std::move(scene), std::move(gfxCache),
                                          camera.entity());
  return std::make_shared<SimpleSceneView>(
    std::move(sceneView), cylinder.entity(), material, tciMaterial);
}
