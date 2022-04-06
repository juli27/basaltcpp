#include <basalt/sandbox/samples/lighting.h>
#include <basalt/sandbox/samples/samples.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>
#include <basalt/api/scene_view.h>

#include <basalt/api/gfx/camera.h>
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
using basalt::EntityName;
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
using basalt::gfx::ext::XModelHandle;

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
  // sphere models
  auto materials = array<MaterialHandle, 1>{};
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
  materialDesc.sampledTexture.texture =
    mGfxCache->load_texture_2d(SPHERE_TEXTURE_FILE_PATH);
  materialDesc.sampledTexture.sampler =
    mGfxCache->create_sampler({TextureFilter::Bilinear, TextureFilter::Bilinear,
                               TextureMipFilter::Linear});
  materialDesc.fogColor = BACKGROUND;
  materialDesc.fogDensity = 0.01f;
  auto sphereModels = array<XModelHandle, 10>{};
  for (auto i = uSize{0}; i < 10; i++) {
    auto const perSphereFactor = static_cast<f32>(i);
    materialDesc.diffuse = Color::from_non_linear(0.75f, 0.75f, 0.75f);
    materialDesc.ambient = Color::from_non_linear(0.25f, 0.25f, 0.25f);
    materialDesc.emissive = Colors::BLACK;
    materialDesc.specular = Color::from_non_linear(0.25f, 0.25f, 0.25f);
    materialDesc.specularPower = 5 * perSphereFactor;
    std::get<0>(materials) = mGfxCache->create_material(materialDesc);
    sphereModels[i] = mGfxCache->load_x_model(
      {"data/tribase/02-07_lighting/Sphere.x"sv, materials});
  }

  // ground model
  materialDesc.diffuse = Color::from_non_linear(0.75f, 0.75f, 0.75f);
  materialDesc.ambient = Color::from_non_linear(0.25f, 0.25f, 0.25f);
  materialDesc.emissive = Colors::BLACK;
  materialDesc.specular = Colors::WHITE;
  materialDesc.specularPower = 1.0f;
  materialDesc.sampledTexture.texture =
    mGfxCache->load_texture_2d(GROUND_TEXTURE_FILE_PATH);
  std::get<0>(materials) = mGfxCache->create_material(materialDesc);
  auto const groundModel = mGfxCache->load_x_model(
    {"data/tribase/02-07_lighting/Ground.x"sv, materials});

  // light model
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
  std::get<0>(materials) = mGfxCache->create_material(materialDesc);
  auto const lightModel = mGfxCache->load_x_model(
    {"data/tribase/02-07_lighting/Sphere.x"sv, materials});

  auto scene = Scene::create();
  auto& gfxEnv = scene->entity_registry().ctx().emplace<Environment>();
  gfxEnv.set_background(BACKGROUND);
  gfxEnv.set_ambient_light(Color::from_non_linear(0.25f, 0, 0));

  mCenter = scene->create_entity();
  mCenter.emplace<EntityName>("Center"s);

  for (auto i = uSize{0}; i < 10; i++) {
    auto const perSphereFactor = static_cast<f32>(i);

    // evenly space out the spheres at the edge of a circle with radius 10
    // around the center
    auto const angle = Angle::degrees(36 * perSphereFactor);
    auto const pos = Vector3f32{10 * angle.cos(), 0, 10 * angle.sin()};
    // make the spheres point inward
    auto const rotation = Vector3f32{0, -1 * angle.radians(), 0};
    mSpheres[i] = scene->create_entity(pos, rotation);
    mSpheres[i].emplace<EntityName>(
      fmt::format(FMT_STRING("Sphere {}"), i + 1));
    mSpheres[i].emplace<Parent>(mCenter.entity());

    mSpheres[i].emplace<XModelHandle>(sphereModels[i]);
  }

  mGround = scene->create_entity({0, -50, 0});
  mGround.emplace<EntityName>("Ground"s);
  (void)mGround.emplace<XModelHandle>(groundModel);

  mLight = scene->create_entity();
  mLight.emplace<EntityName>("Light"s);
  (void)mLight.emplace<XModelHandle>(lightModel);
  mLight.emplace<Light>(PointLight{Colors::WHITE, Colors::WHITE, Colors::WHITE,
                                   1000.0f, 0.0f, 0.025f, 0.0f});

  auto const cameraEntity = scene->create_entity({5, 7.5f, -15});
  cameraEntity.emplace<EntityName>("Camera"s);
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
