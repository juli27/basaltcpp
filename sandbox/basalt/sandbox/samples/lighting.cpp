#include <basalt/sandbox/samples/lighting.h>

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

#include <array>
#include <cmath>
#include <utility>

namespace samples {

using std::array;

using namespace basalt::literals;
using basalt::Angle;
using basalt::Engine;
using basalt::Parent;
using basalt::Scene;
using basalt::SceneView;
using basalt::Transform;
using basalt::Vector3f32;
using basalt::gfx::Camera;
using basalt::gfx::CullMode;
using basalt::gfx::Environment;
using basalt::gfx::FogMode;
using basalt::gfx::FogType;
using basalt::gfx::MaterialDescriptor;
using basalt::gfx::PipelineDescriptor;
using basalt::gfx::PointLightComponent;
using basalt::gfx::TestPassCond;
using basalt::gfx::TextureBlendingStage;
using basalt::gfx::TextureFilter;
using basalt::gfx::TextureMipFilter;
using basalt::gfx::TextureOp;
using basalt::gfx::XModelDescriptor;
using basalt::gfx::ext::XModel;

namespace {

constexpr Color BACKGROUND {Color::from_non_linear_rgba8(0, 0, 63)};

} // namespace

Lighting::Lighting(Engine& engine)
  : mGfxCache {engine.create_gfx_resource_cache()} {
  array<TextureBlendingStage, 1> textureStages {};
  PipelineDescriptor pipelineDesc;
  pipelineDesc.textureStages = textureStages;
  pipelineDesc.lightingEnabled = true;
  pipelineDesc.specularEnabled = true;
  pipelineDesc.cullMode = CullMode::CounterClockwise;
  pipelineDesc.depthTest = TestPassCond::IfLessEqual;
  pipelineDesc.depthWriteEnable = true;
  pipelineDesc.dithering = true;
  pipelineDesc.fogType = FogType::Vertex;
  pipelineDesc.fogMode = FogMode::Exponential;

  // sphere models
  const auto sphereTexture =
    mGfxCache->load_texture("data/tribase/02-07_lighting/Sphere.bmp");
  array<MaterialDescriptor, 1> materials {};
  MaterialDescriptor& materialDesc {std::get<0>(materials)};
  materialDesc.pipelineDesc = &pipelineDesc;
  materialDesc.sampledTexture.texture = sphereTexture;
  materialDesc.sampledTexture.filter = TextureFilter::Bilinear;
  materialDesc.sampledTexture.mipFilter = TextureMipFilter::Linear;
  materialDesc.fogColor = BACKGROUND;
  materialDesc.fogDensity = 0.01f;
  array<XModel, 10> sphereModels {};
  for (i32 i {0}; i < 10; i++) {
    const f32 perSphereFactor {static_cast<f32>(i)};
    materialDesc.diffuse = Color::from_non_linear(0.75f, 0.75f, 0.75f);
    materialDesc.ambient = Color::from_non_linear(0.25f, 0.25f, 0.25f);
    materialDesc.emissive = Colors::BLACK;
    materialDesc.specular = Color::from_non_linear(0.25f, 0.25f, 0.25f);
    materialDesc.specularPower = 5 * perSphereFactor;

    sphereModels[i] = mGfxCache->load_x_model(
      XModelDescriptor {"data/tribase/02-07_lighting/Sphere.x", materials});
  }

  // ground model
  const auto groundTexture {
    mGfxCache->load_texture("data/tribase/02-07_lighting/Ground.bmp")};
  materialDesc.diffuse = Color::from_non_linear(0.75f, 0.75f, 0.75f);
  materialDesc.ambient = Color::from_non_linear(0.25f, 0.25f, 0.25f);
  materialDesc.emissive = Colors::BLACK;
  materialDesc.specular = Colors::WHITE;
  materialDesc.specularPower = 1.0f;
  materialDesc.sampledTexture.texture = groundTexture;
  materialDesc.sampledTexture.filter = TextureFilter::Bilinear;
  materialDesc.sampledTexture.mipFilter = TextureMipFilter::Linear;
  const auto groundModel {mGfxCache->load_x_model(
    XModelDescriptor {"data/tribase/02-07_lighting/Ground.x", materials})};

  // light model
  std::get<0>(textureStages).colorOp = TextureOp::SelectArg2;
  std::get<0>(textureStages).alphaOp = TextureOp::SelectArg2;
  materialDesc.sampledTexture = {};
  materialDesc.diffuse = Color::from_non_linear(0.75f, 0.75f, 0.75f);
  materialDesc.ambient = Color::from_non_linear(0.25f, 0.25f, 0.25f);
  materialDesc.emissive = Colors::WHITE;
  materialDesc.specular = Colors::WHITE;
  materialDesc.specularPower = 1.0f;
  const auto lightModel {mGfxCache->load_x_model(
    XModelDescriptor {"data/tribase/02-07_lighting/Sphere.x", materials})};

  auto scene {Scene::create()};
  auto& gfxEnv {scene->entity_registry().ctx().emplace<Environment>()};
  gfxEnv.set_background(BACKGROUND);
  gfxEnv.set_ambient_light(Color::from_non_linear(0.25f, 0, 0));

  mCenter = scene->create_entity();

  for (i32 i {0}; i < 10; i++) {
    const f32 perSphereFactor {static_cast<f32>(i)};

    // evenly space out the spheres at the edge of a circle with radius 10
    // around the center
    Angle angle {Angle::degrees(36 * perSphereFactor)};
    Vector3f32 pos {10 * angle.cos(), 0, 10 * angle.sin()};
    // make the spheres point inward
    Vector3f32 rotation {0, -1 * angle.radians(), 0};
    mSpheres[i] = scene->create_entity(pos, rotation);
    mSpheres[i].emplace<Parent>(mCenter.entity());

    mSpheres[i].emplace<XModel>(sphereModels[i]);
  }

  mGround = scene->create_entity({0, -50, 0});
  (void)mGround.emplace<XModel>(groundModel);

  mLight = scene->create_entity();
  (void)mLight.emplace<XModel>(lightModel);
  mLight.emplace<PointLightComponent>(
    Colors::WHITE, Colors::WHITE, Colors::WHITE, 1000.0f, 0.0f, 0.025f, 0.0f);

  const auto cameraEntity {scene->create_entity({5, 7.5f, -15})};
  cameraEntity.emplace<Camera>(
    Camera {{}, Vector3f32::up(), 90_deg, 0.1f, 500});

  add_child_top(SceneView::create(std::move(scene), mGfxCache, cameraEntity));
}

auto Lighting::on_update(UpdateContext& ctx) -> void {
  mTime += ctx.deltaTime;
  const f32 t {mTime.count()};

  mCenter.get<Transform>().rotation.y() = Angle::degrees(t * 10).radians();

  for (i32 i {0}; i < 10; i++) {
    const f32 perSphereFactor {static_cast<f32>(i)};

    mSpheres[i].get<Transform>().position.y() =
      std::sin(perSphereFactor + t * 2);
  }

  mGround.get<Transform>().rotation.y() = Angle::radians(t).radians();

  const Vector3f32 lightPos {0, std::sin(t) * 10, 0};
  mLight.get<Transform>().position = lightPos;
}

} // namespace samples
