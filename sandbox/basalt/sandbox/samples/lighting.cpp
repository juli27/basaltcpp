#include <basalt/sandbox/samples/lighting.h>
#include <basalt/sandbox/samples/samples.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>
#include <basalt/api/scene_view.h>

#include <basalt/api/gfx/camera.h>
#include <basalt/api/gfx/context.h>
#include <basalt/api/gfx/environment.h>
#include <basalt/api/gfx/resource_cache.h>

#include <basalt/api/scene/scene.h>
#include <basalt/api/scene/transform.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/vector3.h>

#include <fmt/format.h>

#include <array>
#include <cmath>
#include <string>
#include <string_view>
#include <utility>

using namespace std::literals;
using std::array;

using namespace basalt::literals;
using basalt::Angle;
using basalt::Engine;
using basalt::Parent;
using basalt::Scene;
using basalt::SceneView;
using basalt::Transform;
using basalt::Vector3f32;
using basalt::ViewPtr;
using basalt::gfx::Camera;
using basalt::gfx::CullMode;
using basalt::gfx::Environment;
using basalt::gfx::FixedFragmentShaderCreateInfo;
using basalt::gfx::FixedVertexShaderCreateInfo;
using basalt::gfx::FogMode;
using basalt::gfx::Light;
using basalt::gfx::MaterialCreateInfo;
using basalt::gfx::MaterialHandle;
using basalt::gfx::PipelineCreateInfo;
using basalt::gfx::PointLight;
using basalt::gfx::TestPassCond;
using basalt::gfx::TextureFilter;
using basalt::gfx::TextureMipFilter;
using basalt::gfx::TextureStage;
using basalt::gfx::ext::XMeshHandle;
using basalt::gfx::ext::XModel;

namespace {

constexpr auto BACKGROUND = Color::from_non_linear_rgba8(0, 0, 63);
constexpr auto GROUND_TEXTURE_FILE_PATH =
  "data/tribase/02-07_lighting/Ground.bmp"sv;
constexpr auto SPHERE_TEXTURE_FILE_PATH =
  "data/tribase/02-07_lighting/Sphere.bmp"sv;

} // namespace

namespace samples {

Lighting::Lighting(Engine& engine)
  : mGfxCache{engine.create_gfx_resource_cache()} {
  auto scene = Scene::create();
  auto& gfxEnv = scene->entity_registry().ctx().emplace<Environment>();
  gfxEnv.set_background(BACKGROUND);
  gfxEnv.set_ambient_light(Color::from_non_linear(0.25f, 0, 0));

  auto materialDesc = MaterialCreateInfo{};
  materialDesc.pipeline = [&] {
    auto vs = FixedVertexShaderCreateInfo{};
    vs.lightingEnabled = true;
    vs.specularEnabled = true;
    vs.fog = FogMode::Exponential;

    auto fs = FixedFragmentShaderCreateInfo{};
    constexpr auto textureStages = array{TextureStage{}};
    fs.textureStages = textureStages;

    auto pipelineDesc = PipelineCreateInfo{};
    pipelineDesc.vertexShader = &vs;
    pipelineDesc.fragmentShader = &fs;
    pipelineDesc.cullMode = CullMode::CounterClockwise;
    pipelineDesc.depthTest = TestPassCond::IfLessEqual;
    pipelineDesc.depthWriteEnable = true;
    pipelineDesc.dithering = true;

    return mGfxCache->create_pipeline(pipelineDesc);
  }();
  materialDesc.sampledTexture.sampler =
    mGfxCache->create_sampler({TextureFilter::Bilinear, TextureFilter::Bilinear,
                               TextureMipFilter::Linear});
  materialDesc.fogColor = BACKGROUND;
  materialDesc.fogDensity = 0.01f;

  auto const sphereMesh = [&] {
    auto const data =
      mGfxCache->load_x_meshes("data/tribase/02-07_lighting/Sphere.x"sv);

    return data.meshes.front();
  }();

  mCenter = scene->create_entity("Center"s);
  mSpheres = [&] {
    auto entities = array<basalt::Entity, 10>{};

    materialDesc.sampledTexture.texture =
      mGfxCache->load_texture_2d(SPHERE_TEXTURE_FILE_PATH);
    materialDesc.diffuse = Color::from_non_linear(0.75f, 0.75f, 0.75f);
    materialDesc.ambient = Color::from_non_linear(0.25f, 0.25f, 0.25f);
    materialDesc.emissive = Colors::BLACK;
    materialDesc.specular = Color::from_non_linear(0.25f, 0.25f, 0.25f);

    for (auto i = uSize{0}; i < 10; i++) {
      entities[i] = [&] {
        auto const perSphereFactor = static_cast<f32>(i);
        auto sphereName = fmt::format(FMT_STRING("Sphere {}"), i + 1);
        // evenly space out the spheres at the edge of a circle with radius 10
        // around the center
        auto const angle = Angle::degrees(36 * perSphereFactor);
        auto const pos = Vector3f32{10 * angle.cos(), 0, 10 * angle.sin()};
        // make the spheres point inward
        auto const rotation = Vector3f32{0, -1 * angle.radians(), 0};
        auto entity =
          scene->create_entity(std::move(sphereName), pos, rotation);
        entity.emplace<Parent>(mCenter.entity());

        auto const sphereMaterial = [&] {
          materialDesc.specularPower = 5 * perSphereFactor;

          return mGfxCache->create_material(materialDesc);
        }();
        entity.emplace<XModel>(sphereMaterial, sphereMesh);

        return entity;
      }();
    }

    return entities;
  }();
  mGround = [&] {
    auto entity = scene->create_entity("Ground"s, Vector3f32{0, -50, 0});
    auto const groundMaterial = [&] {
      materialDesc.diffuse = Color::from_non_linear(0.75f, 0.75f, 0.75f);
      materialDesc.ambient = Color::from_non_linear(0.25f, 0.25f, 0.25f);
      materialDesc.emissive = Colors::BLACK;
      materialDesc.specular = Colors::WHITE;
      materialDesc.specularPower = 1.0f;
      materialDesc.sampledTexture.texture =
        mGfxCache->load_texture_2d(GROUND_TEXTURE_FILE_PATH);

      return mGfxCache->create_material(materialDesc);
    }();
    auto const groundMesh = [&] {
      auto const data =
        mGfxCache->load_x_meshes("data/tribase/02-07_lighting/Ground.x"sv);

      return data.meshes.front();
    }();
    entity.emplace<XModel>(groundMaterial, groundMesh);

    return entity;
  }();
  mLight = [&] {
    auto entity = scene->create_entity("Light"s);
    auto const lightMaterial = [&] {
      materialDesc.pipeline = [&] {
        auto vs = FixedVertexShaderCreateInfo{};
        vs.lightingEnabled = true;
        vs.specularEnabled = true;
        vs.fog = FogMode::Exponential;

        auto pipelineDesc = PipelineCreateInfo{};
        pipelineDesc.vertexShader = &vs;
        pipelineDesc.cullMode = CullMode::CounterClockwise;
        pipelineDesc.depthTest = TestPassCond::IfLessEqual;
        pipelineDesc.depthWriteEnable = true;
        pipelineDesc.dithering = true;

        return mGfxCache->create_pipeline(pipelineDesc);
      }();
      materialDesc.sampledTexture = {};
      materialDesc.diffuse = Color::from_non_linear(0.75f, 0.75f, 0.75f);
      materialDesc.ambient = Color::from_non_linear(0.25f, 0.25f, 0.25f);
      materialDesc.emissive = Colors::WHITE;
      materialDesc.specular = Colors::WHITE;
      materialDesc.specularPower = 1.0f;

      return mGfxCache->create_material(materialDesc);
    }();
    entity.emplace<XModel>(lightMaterial, sphereMesh);

    entity.emplace<Light>(PointLight{Colors::WHITE, Colors::WHITE,
                                     Colors::WHITE, 1000.0f, 0.0f, 0.025f,
                                     0.0f});

    return entity;
  }();

  auto const cameraEntity =
    scene->create_entity("Camera"s, Vector3f32{5, 7.5f, -15});
  cameraEntity.emplace<Camera>(Camera{{}, Vector3f32::up(), 90_deg, 0.1f, 500});

  add_child_top(SceneView::create(std::move(scene), mGfxCache, cameraEntity));
}

auto Lighting::on_update(UpdateContext& ctx) -> void {
  mTime += ctx.deltaTime;
  auto const t = mTime.count();

  mCenter.get<Transform>().rotation.y() = Angle::degrees(t * 10).radians();

  for (auto i = uSize{0}; i < 10; i++) {
    auto const perSphereFactor = static_cast<f32>(i);

    mSpheres[i].get<Transform>().position.y() =
      std::sin(perSphereFactor + t * 2);
  }

  mGround.get<Transform>().rotation.y() = Angle::radians(t).radians();
  mLight.get<Transform>().position.y() = std::sin(t) * 10;
}

} // namespace samples

auto Samples::new_lighting_sample(Engine& engine) -> ViewPtr {
  return std::make_shared<samples::Lighting>(engine);
}
