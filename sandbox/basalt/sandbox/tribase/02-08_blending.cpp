#include <basalt/sandbox/tribase/02-08_blending.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/ext/x_model_support.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/matrix4x4.h>
#include <basalt/api/math/vector3.h>

#include <fmt/format.h>
#include <gsl/span>

#include <algorithm>
#include <array>
#include <cmath>
#include <random>
#include <vector>

namespace tribase {

using std::array;
using std::default_random_engine;
using std::random_device;
using std::uniform_real_distribution;
using std::vector;

using gsl::span;

using namespace basalt::literals;
using basalt::Angle;
using basalt::Engine;
using basalt::Matrix4x4f32;
using basalt::Vector3f32;
using basalt::gfx::Attachment;
using basalt::gfx::Attachments;
using basalt::gfx::BlendFactor;
using basalt::gfx::CommandList;
using basalt::gfx::CullMode;
using basalt::gfx::LightData;
using basalt::gfx::PipelineDescriptor;
using basalt::gfx::PointLightData;
using basalt::gfx::SamplerDescriptor;
using basalt::gfx::TextureBlendingStage;
using basalt::gfx::TextureFilter;
using basalt::gfx::TextureMipFilter;
using basalt::gfx::TextureOp;
using basalt::gfx::TransformState;
using basalt::gfx::VertexElement;
using basalt::gfx::ext::XMeshCommandEncoder;

namespace {

using Distribution = uniform_real_distribution<float>;

struct StarVertex {
  Vector3f32 position {};
  ColorEncoding::A8R8G8B8 diffuse {};

  static constexpr array sLayout {VertexElement::Position3F32,
                                  VertexElement::ColorDiffuse1U32A8R8G8B8};
};

struct Planet {
  Vector3f32 position;
  f32 radius;
  f32 distanceToCamera;
  u8 type;
};

constexpr Color BACKGROUND {Colors::BLACK};
constexpr u32 NUM_STARS {1024};

auto generate_stars(const span<StarVertex> stars) -> void {
  default_random_engine randomEngine {random_device {}()};
  Distribution rng1 {-1.0f, 1.0f};
  Distribution rng2 {0.1f, 1.0f};

  const auto normalizedRandomVector {[&] {
    return Vector3f32::normalize(rng1(randomEngine), rng1(randomEngine),
                                 rng1(randomEngine));
  }};

  for (auto& starVertex : stars) {
    starVertex.position = normalizedRandomVector() * 100.0f;

    const f32 c {rng2(randomEngine)};
    starVertex.diffuse = Color::from_non_linear(c, c, c).to_argb();
  }
}

// this is not a complete vector transform function (only considers upper-left
// 3x3 matrix)
auto operator*(const Vector3f32& v, const Matrix4x4f32& m) -> Vector3f32 {
  return {v.x() * m.m11 + v.y() * m.m21 + v.z() * m.m31,
          v.x() * m.m12 + v.y() * m.m22 + v.z() * m.m32,
          v.x() * m.m13 + v.y() * m.m23 + v.z() * m.m33};
}

auto update_planets(const span<Planet, 6> planets, const f32 t) -> void {
  // the sun
  auto& planet0 {planets[0]};
  planet0.position = {};
  planet0.radius = 7;

  auto& planet1 {planets[1]};
  planet1.position =
    10.0f * Vector3f32 {std::sin(1.5f * t), 0, std::cos(1.5f * t)};
  planet1.radius = 1;

  // elliptical orbit and 20 degree tilt
  auto& planet2 {planets[2]};
  planet2.position =
    16.0f * Vector3f32 {0.8f * std::sin(0.75f * t), 0, std::cos(0.75f * t)} *
    Matrix4x4f32::rotation_x(20_deg);
  planet2.radius = 1.5f;

  // elliptical orbit and 30 degree tilt
  auto& planet3 {planets[3]};
  planet3.position =
    22.0f * Vector3f32 {std::sin(0.5f * t), 0, 0.75f * std::cos(0.5f * t)} *
    Matrix4x4f32::rotation_z(30_deg);
  planet3.radius = 2.0f;

  auto& planet4 {planets[4]};
  planet4.position =
    30.0f * Vector3f32 {std::sin(0.25f * t), 0, std::cos(0.25f * t)};
  planet4.radius = 3.0f;

  // elliptical orbit
  auto& planet5 {planets[5]};
  planet5.position =
    70.0f * Vector3f32 {std::sin(0.75f * t), 0, 0.5f * std::cos(0.75f * t)};
  planet5.radius = 1.5f;
}

auto rotation_axis(Vector3f32 axis, const Angle angle) -> Matrix4x4f32 {
  const float sin = std::sin(-angle.radians());
  const float cos = std::cos(-angle.radians());
  const float oneMinusCos = 1.0f - cos;

  axis = axis.normalize();

  return {(axis.x() * axis.x()) * oneMinusCos + cos,
          (axis.x() * axis.y()) * oneMinusCos - (axis.z() * sin),
          (axis.x() * axis.z()) * oneMinusCos + (axis.y() * sin),
          0.0f,

          (axis.y() * axis.x()) * oneMinusCos + (axis.z() * sin),
          (axis.y() * axis.y()) * oneMinusCos + cos,
          (axis.y() * axis.z()) * oneMinusCos - (axis.x() * sin),
          0.0f,

          (axis.z() * axis.x()) * oneMinusCos - (axis.y() * sin),
          (axis.z() * axis.y()) * oneMinusCos + (axis.x() * sin),
          (axis.z() * axis.z()) * oneMinusCos + cos,
          0.0f,

          0.0f,
          0.0f,
          0.0f,
          1.0f};
}

} // namespace

Blending::Blending(const Engine& engine)
  : mGfxCache {engine.create_gfx_resource_cache()} {
  PipelineDescriptor starPipelineDesc {};
  starPipelineDesc.vertexInputState = StarVertex::sLayout;
  starPipelineDesc.dithering = true;
  mStarPipeline = mGfxCache->create_pipeline(starPipelineDesc);

  PipelineDescriptor planetPipelineDesc {};
  array textureStages {TextureBlendingStage {}};
  std::get<0>(textureStages).alphaOp = TextureOp::Modulate;
  planetPipelineDesc.textureStages = textureStages;
  planetPipelineDesc.lightingEnabled = true;
  planetPipelineDesc.cullMode = CullMode::Clockwise;
  planetPipelineDesc.dithering = true;
  planetPipelineDesc.srcBlendFactor = BlendFactor::SrcAlpha;
  planetPipelineDesc.destBlendFactor = BlendFactor::OneMinusSrcAlpha;
  mPlanetPipelineCw = mGfxCache->create_pipeline(planetPipelineDesc);
  planetPipelineDesc.cullMode = CullMode::CounterClockwise;
  mPlanetPipelineCcw = mGfxCache->create_pipeline(planetPipelineDesc);

  SamplerDescriptor samplerDesc {};
  samplerDesc.magFilter = TextureFilter::Bilinear;
  samplerDesc.minFilter = TextureFilter::Bilinear;
  samplerDesc.mipFilter = TextureMipFilter::Linear;
  mSampler = mGfxCache->create_sampler(samplerDesc);

  for (u32 i {0}; i < mPlanetTextures.size(); i++) {
    mPlanetTextures[i] = mGfxCache->load_texture(
      fmt::format(FMT_STRING("data/tribase/02-08_blending/Planet{}.dds"), i));
  }

  mPlanetModel =
    mGfxCache->load_x_model("data/tribase/02-08_blending/Planet.x");
  mSunModel = mGfxCache->load_x_model("data/tribase/02-08_blending/Sun.x");

  vector<StarVertex> starVertices(NUM_STARS);
  generate_stars(starVertices);
  const auto starVertexData {as_bytes(span {starVertices})};
  mStarsVb = mGfxCache->create_vertex_buffer(
    {starVertexData.size(), StarVertex::sLayout}, starVertexData);
}

auto Blending::on_update(UpdateContext& ctx) -> void {
  mTime += ctx.deltaTime;
  const f32 t {mTime.count()};

  const Vector3f32 cameraPos {50.0f * std::sin(0.35f * t),
                              50.0f * std::cos(0.2f * t),
                              40.0f * std::sin(0.1f * t) - 10.0f};

  array<Planet, 6> planets {};
  update_planets(planets, t);

  for (u32 i {0}; i < planets.size(); ++i) {
    planets[i].type = static_cast<u8>(i);
    planets[i].distanceToCamera = (planets[i].position - cameraPos).length();
  }

  // sort the transparent planets from back to front
  std::sort(planets.begin(), planets.end(),
            [](const Planet& p1, const Planet& p2) {
              const f32 diff {p1.distanceToCamera - p2.distanceToCamera};

              // if p1 is further away from the camera than p2 then render p1
              // before p2
              return diff > 0.0f;
            });

  const auto& drawCtx {ctx.drawCtx};
  CommandList cmdList;
  cmdList.clear_attachments(Attachments {Attachment::RenderTarget}, BACKGROUND);

  cmdList.bind_pipeline(mStarPipeline);
  cmdList.set_transform(TransformState::ViewToClip,
                        Matrix4x4f32::perspective_projection(
                          90_deg, drawCtx.viewport.aspect_ratio(), 0.1f, 200));
  cmdList.set_transform(
    TransformState::WorldToView,
    Matrix4x4f32::look_at_lh(cameraPos, {}, Vector3f32::up()));
  cmdList.set_transform(TransformState::LocalToWorld, Matrix4x4f32::identity());
  cmdList.bind_vertex_buffer(mStarsVb);
  cmdList.draw(0, NUM_STARS);

  cmdList.bind_sampler(mSampler);

  constexpr array<LightData, 1> lights {
    PointLightData {Color::from_non_linear(1.0f, 1.0f, 0.75f),
                    {},
                    Color::from_non_linear(1.0f, 1.0f, 0.75f),
                    {},
                    1000,
                    1.0f,
                    0,
                    0}};
  cmdList.set_lights(lights);

  const auto& planetModel {mGfxCache->get(mPlanetModel)};
  for (const auto& planet : planets) {
    if (planet.type == 0) {
      // this is the sun

      default_random_engine randomEngine {random_device {}()};
      Distribution rng {0.0f, 0.025f};
      for (i32 i {0}; i < 21; i++) {
        const auto localToWorld {
          Matrix4x4f32::rotation_x(Angle::radians(
            (static_cast<f32>(i - 10) + rng(randomEngine)) * t / 20.0f)) *
          Matrix4x4f32::rotation_y(Angle::radians(
            (static_cast<f32>(-i - 10) + rng(randomEngine)) * t / 20.0f)) *
          Matrix4x4f32::rotation_z(Angle::radians(
            (static_cast<f32>(i - 10) + rng(randomEngine)) * t / 20.0f)) *
          Matrix4x4f32::scaling(
            Vector3f32 {planet.radius + static_cast<f32>(i) / 10.0f} +
            Vector3f32 {0.25f * std::sin(4.0f * t)})};
        cmdList.set_transform(TransformState::LocalToWorld, localToWorld);
        cmdList.set_material(
          Color::from_non_linear(0, 0, 0, 1.1f - static_cast<f32>(i) / 20.0f),
          {}, Color::from_non_linear(1.0f, 1.0f, 0.75f));

        cmdList.bind_texture(std::get<0>(mPlanetTextures));

        const auto& sunModel {mGfxCache->get(mSunModel)};
        cmdList.bind_pipeline(mPlanetPipelineCw);
        XMeshCommandEncoder::draw_x_mesh(cmdList, sunModel.meshes[0]);
        cmdList.bind_pipeline(mPlanetPipelineCcw);
        XMeshCommandEncoder::draw_x_mesh(cmdList, sunModel.meshes[0]);
      }

      continue;
    }

    const auto localToWorld {
      Matrix4x4f32::scaling(planet.radius) *
      rotation_axis({0.5f, 1.0f, 0.25f}, Angle::radians(t)) *
      Matrix4x4f32::translation(planet.position)};
    cmdList.set_transform(TransformState::LocalToWorld, localToWorld);
    cmdList.set_material(Colors::WHITE,
                         Color::from_non_linear(0.25f, 0.25f, 0.25f));
    cmdList.bind_texture(mPlanetTextures[planet.type]);

    cmdList.bind_pipeline(mPlanetPipelineCw);
    XMeshCommandEncoder::draw_x_mesh(cmdList, planetModel.meshes[0]);
    cmdList.bind_pipeline(mPlanetPipelineCcw);
    XMeshCommandEncoder::draw_x_mesh(cmdList, planetModel.meshes[0]);
  }

  drawCtx.commandLists.emplace_back(std::move(cmdList));
}

} // namespace tribase
