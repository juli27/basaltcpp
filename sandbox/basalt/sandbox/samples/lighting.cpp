#include "samples.h"

#include <basalt/sandbox/shared/debug_scene_view.h>
#include <basalt/sandbox/shared/rotation_system.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/camera.h>
#include <basalt/api/gfx/context.h>
#include <basalt/api/gfx/environment.h>
#include <basalt/api/gfx/material.h>
#include <basalt/api/gfx/material_class.h>
#include <basalt/api/gfx/resource_cache.h>

#include <basalt/api/scene/scene.h>
#include <basalt/api/scene/system.h>
#include <basalt/api/scene/transform.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/vector3.h>

#include <fmt/format.h>

#include <array>
#include <cmath>
#include <string>
#include <string_view>
#include <utility>

using namespace basalt;
using namespace std::literals;

namespace {

auto constexpr BACKGROUND = Color::from_non_linear_rgba8(0, 0, 63);
auto constexpr GROUND_TEXTURE_FILE_PATH =
  "data/tribase/02-07_lighting/Ground.bmp"sv;
auto constexpr GROUND_MESH_FILE_PATH = "data/tribase/02-07_lighting/Ground.x"sv;
auto constexpr SPHERE_TEXTURE_FILE_PATH =
  "data/tribase/02-07_lighting/Sphere.bmp"sv;
auto constexpr SPHERE_MESH_FILE_PATH = "data/tribase/02-07_lighting/Sphere.x"sv;

struct Bobbing {
  f32 extent{1.0f};
  f32 angleFactor{1.0f};
  f32 angleOffset{0.0f};
};

class BobbingSystem final : public System {
public:
  using UpdateBefore = TransformSystem;

  auto on_update(UpdateContext const& ctx) -> void override {
    auto& ecs = ctx.scene.entity_registry();
    auto const view = ecs.view<Transform, Bobbing const>();

    auto const t = ctx.time.count();
    view.each([&](Transform& transform, Bobbing const& bobbing) {
      transform.position.y() =
        bobbing.extent *
        std::sin(bobbing.angleFactor * t + bobbing.angleOffset);
    });
  }
};

} // namespace

auto Samples::new_lighting_sample(Engine& engine) -> ViewPtr {
  auto scene = Scene::create();
  scene->create_system<RotationSystem>();
  scene->create_system<BobbingSystem>();

  auto& gfxEnv = scene->entity_registry().ctx().emplace<gfx::Environment>();
  gfxEnv.set_background(BACKGROUND);
  gfxEnv.set_ambient_light(Color::from_non_linear(0.25f, 0, 0));

  auto gfxCache = engine.create_gfx_resource_cache();

  auto const materialClass = [&] {
    auto info = gfx::MaterialClassCreateInfo{};
    auto& pipelineInfo = info.pipelineInfo;

    auto vs = gfx::FixedVertexShaderCreateInfo{};
    vs.lightingEnabled = true;
    vs.specularEnabled = true;
    vs.fog = gfx::FogMode::Exponential;
    pipelineInfo.vertexShader = &vs;

    auto fs = gfx::FixedFragmentShaderCreateInfo{};
    constexpr auto textureStages = std::array{gfx::TextureStage{}};
    fs.textureStages = textureStages;
    pipelineInfo.fragmentShader = &fs;

    pipelineInfo.cullMode = gfx::CullMode::CounterClockwise;
    pipelineInfo.depthTest = gfx::TestPassCond::IfLessEqual;
    pipelineInfo.depthWriteEnable = true;

    return gfxCache->create_material_class(info);
  }();

  auto constexpr fogParams = gfx::FogParameters{BACKGROUND, 0.0f, 0.0f, 0.01f};
  auto const sampler = [&] {
    auto info = gfx::SamplerCreateInfo{};
    info.magFilter = gfx::TextureFilter::Bilinear;
    info.minFilter = gfx::TextureFilter::Bilinear;
    info.mipFilter = gfx::TextureMipFilter::Linear;

    return gfxCache->create_sampler(info);
  }();

  {
    auto const ground =
      scene->create_entity("Ground"s, Vector3f32{0.0f, -50.0f, 0.0f});
    ground.emplace<RotationSpeed>(0.0f, 1.0f, 0.0f);

    auto const material = [&] {
      auto info = gfx::MaterialCreateInfo{};
      info.clazz = materialClass;

      auto uniformColors = gfx::UniformColors{};
      uniformColors.diffuse = Color::from_non_linear(0.75f, 0.75f, 0.75f);
      uniformColors.ambient = Color::from_non_linear(0.25f, 0.25f, 0.25f);
      uniformColors.emissive = Colors::BLACK;
      uniformColors.specular = Colors::WHITE;
      uniformColors.specularPower = 1.0f;

      auto const texture = gfxCache->load_texture_2d(GROUND_TEXTURE_FILE_PATH);

      auto const properties = std::array{
        gfx::MaterialProperty{gfx::MaterialPropertyId::FogParameters,
                              fogParams},
        gfx::MaterialProperty{gfx::MaterialPropertyId::SampledTexture,
                              gfx::SampledTexture{sampler, texture}},
        gfx::MaterialProperty{gfx::MaterialPropertyId::UniformColors,
                              uniformColors},
      };
      info.initialValues = properties;

      return gfxCache->create_material(info);
    }();
    auto const mesh = [&] {
      auto const data = gfxCache->load_x_meshes(GROUND_MESH_FILE_PATH);

      return data.meshes.front();
    }();
    ground.emplace<gfx::ext::XModel>(material, mesh);
  }

  auto const sphereMesh = [&] {
    auto const data = gfxCache->load_x_meshes(SPHERE_MESH_FILE_PATH);

    return data.meshes.front();
  }();
  {
    auto const center = scene->create_entity("Center"s);
    center.emplace<RotationSpeed>(0.0f, Angle::degToRad(10.0f), 0.0f);

    auto const texture = gfxCache->load_texture_2d(SPHERE_TEXTURE_FILE_PATH);
    auto uniformColors = gfx::UniformColors{};
    uniformColors.diffuse = Color::from_non_linear(0.75f, 0.75f, 0.75f);
    uniformColors.ambient = Color::from_non_linear(0.25f, 0.25f, 0.25f);
    uniformColors.emissive = Colors::BLACK;
    uniformColors.specular = Color::from_non_linear(0.25f, 0.25f, 0.25f);

    for (auto i = uSize{0}; i < 10; ++i) {
      auto const perSphereFactor = static_cast<f32>(i);
      auto name = fmt::format(FMT_STRING("Sphere {}"), i + 1);
      // evenly space out the spheres at the edge of a circle with radius 10
      // around the center
      auto const angle = Angle::degrees(36 * perSphereFactor);
      auto const pos = Vector3f32{10 * angle.cos(), 0, 10 * angle.sin()};
      // make the spheres point inward
      auto const rotation = Vector3f32{0, -1 * angle.radians(), 0};
      auto sphere = scene->create_entity(std::move(name), pos, rotation);
      sphere.emplace<Parent>(center.entity());
      sphere.emplace<Bobbing>(1.0f, 2.0f, perSphereFactor);

      auto const material = [&] {
        auto info = gfx::MaterialCreateInfo{};
        info.clazz = materialClass;

        uniformColors.specularPower = 5 * perSphereFactor;

        auto const properties = std::array{
          gfx::MaterialProperty{gfx::MaterialPropertyId::FogParameters,
                                fogParams},
          gfx::MaterialProperty{gfx::MaterialPropertyId::SampledTexture,
                                gfx::SampledTexture{sampler, texture}},
          gfx::MaterialProperty{gfx::MaterialPropertyId::UniformColors,
                                uniformColors},
        };
        info.initialValues = properties;

        return gfxCache->create_material(info);
      }();
      sphere.emplace<gfx::ext::XModel>(material, sphereMesh);
    }
  }

  {
    auto light = scene->create_entity("Light"s);
    light.emplace<Bobbing>(10.0f, 1.0f, 0.0f);
    auto const lightMaterial = [&] {
      auto classInfo = gfx::MaterialClassCreateInfo{};
      auto& pipelineInfo = classInfo.pipelineInfo;

      auto vs = gfx::FixedVertexShaderCreateInfo{};
      vs.lightingEnabled = true;
      vs.specularEnabled = true;
      vs.fog = gfx::FogMode::Exponential;
      pipelineInfo.vertexShader = &vs;

      pipelineInfo.cullMode = gfx::CullMode::CounterClockwise;
      pipelineInfo.depthTest = gfx::TestPassCond::IfLessEqual;
      pipelineInfo.depthWriteEnable = true;

      auto info = gfx::MaterialCreateInfo{};
      info.clazz = gfxCache->create_material_class(classInfo);

      auto constexpr colors =
        gfx::UniformColors{Color::from_non_linear(0.75f, 0.75f, 0.75f),
                           Color::from_non_linear(0.25f, 0.25f, 0.25f),
                           Colors::WHITE, Colors::WHITE, 1.0f};
      auto constexpr properties = std::array{
        gfx::MaterialProperty{gfx::MaterialPropertyId::FogParameters,
                              fogParams},
        gfx::MaterialProperty{gfx::MaterialPropertyId::UniformColors, colors},
      };
      info.initialValues = properties;

      return gfxCache->create_material(info);
    }();
    light.emplace<gfx::ext::XModel>(lightMaterial, sphereMesh);

    light.emplace<gfx::Light>(gfx::PointLight{Colors::WHITE, Colors::WHITE,
                                              Colors::WHITE, 1000.0f, 0.0f,
                                              0.025f, 0.0f});
  }

  auto const camera =
    scene->create_entity("Camera"s, Vector3f32{5.0f, 7.5f, -15.0f});
  camera.emplace<gfx::Camera>(Vector3f32::zero(), Vector3f32::up(), 90_deg,
                              0.1f, 500.0f);

  return DebugSceneView::create(std::move(scene), std::move(gfxCache),
                                camera.entity());
}
