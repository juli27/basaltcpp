#include "02-08_blending.h"

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>

#include <basalt/api/gfx/context.h>
#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/backend/command_list.h>
#include <basalt/api/gfx/backend/pipeline.h>
#include <basalt/api/gfx/backend/vertex_layout.h>
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
#include <string_view>
#include <vector>

namespace tribase {

using namespace std::literals;
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
using basalt::gfx::FixedFragmentShaderCreateInfo;
using basalt::gfx::FixedVertexShaderCreateInfo;
using basalt::gfx::LightData;
using basalt::gfx::PipelineCreateInfo;
using basalt::gfx::PointLightData;
using basalt::gfx::SamplerCreateInfo;
using basalt::gfx::TextureFilter;
using basalt::gfx::TextureHandle;
using basalt::gfx::TextureMipFilter;
using basalt::gfx::TextureOp;
using basalt::gfx::TextureStage;
using basalt::gfx::TransformState;
using basalt::gfx::VertexElement;
using basalt::gfx::ext::XMeshCommandEncoder;

namespace {

using Distribution = uniform_real_distribution<float>;

struct StarVertex {
  Vector3f32 position{};
  ColorEncoding::A8R8G8B8 diffuse{};

  static constexpr auto sLayout =
    basalt::gfx::make_vertex_layout<VertexElement::Position3F32,
                                    VertexElement::ColorDiffuse1U32A8R8G8B8>();
};

struct Planet {
  Vector3f32 position;
  f32 radius;
  f32 distanceToCamera;
  u8 type;
};

constexpr auto BACKGROUND = Colors::BLACK;
constexpr auto NUM_STARS = u32{1024};
constexpr auto PLANET_MODEL_FILE_PATH =
  "data/tribase/02-08_blending/Planet.x"sv;
constexpr auto SUN_MODEL_FILE_PATH = "data/tribase/02-08_blending/Sun.x"sv;

auto generate_stars(span<StarVertex> const stars) -> void {
  auto randomEngine = default_random_engine{random_device{}()};
  auto rng1 = Distribution{-1.0f, 1.0f};
  auto rng2 = Distribution{0.1f, 1.0f};

  auto const normalizedRandomVector = [&] {
    return Vector3f32::normalized(rng1(randomEngine), rng1(randomEngine),
                                  rng1(randomEngine));
  };

  for (auto& starVertex : stars) {
    starVertex.position = normalizedRandomVector() * 100.0f;

    auto const diffuse = rng2(randomEngine);
    starVertex.diffuse =
      Color::from_non_linear(diffuse, diffuse, diffuse).to_argb();
  }
}

// this is not a complete vector transform function (only considers upper-left
// 3x3 matrix)
auto operator*(Vector3f32 const& v, Matrix4x4f32 const& m) -> Vector3f32 {
  return {v.x() * m.m11 + v.y() * m.m21 + v.z() * m.m31,
          v.x() * m.m12 + v.y() * m.m22 + v.z() * m.m32,
          v.x() * m.m13 + v.y() * m.m23 + v.z() * m.m33};
}

auto update_planets(span<Planet, 6> const planets, f32 const t) -> void {
  // the sun
  auto& planet0 = planets[0];
  planet0.position = {};
  planet0.radius = 7;

  auto& planet1 = planets[1];
  planet1.position =
    10.0f * Vector3f32{std::sin(1.5f * t), 0, std::cos(1.5f * t)};
  planet1.radius = 1;

  // elliptical orbit and 20 degree tilt
  auto& planet2 = planets[2];
  planet2.position =
    16.0f * Vector3f32{0.8f * std::sin(0.75f * t), 0, std::cos(0.75f * t)} *
    Matrix4x4f32::rotation_x(20_deg);
  planet2.radius = 1.5f;

  // elliptical orbit and 30 degree tilt
  auto& planet3 = planets[3];
  planet3.position =
    22.0f * Vector3f32{std::sin(0.5f * t), 0, 0.75f * std::cos(0.5f * t)} *
    Matrix4x4f32::rotation_z(30_deg);
  planet3.radius = 2.0f;

  auto& planet4 = planets[4];
  planet4.position =
    30.0f * Vector3f32{std::sin(0.25f * t), 0, std::cos(0.25f * t)};
  planet4.radius = 3.0f;

  // elliptical orbit
  auto& planet5 = planets[5];
  planet5.position =
    70.0f * Vector3f32{std::sin(0.75f * t), 0, 0.5f * std::cos(0.75f * t)};
  planet5.radius = 1.5f;
}

auto rotation_axis(Vector3f32 axis, Angle const angle) -> Matrix4x4f32 {
  auto const sin = std::sin(-angle.radians());
  auto const cos = std::cos(-angle.radians());
  auto const oneMinusCos = 1.0f - cos;

  axis = axis.normalize();

  return Matrix4x4f32{axis.x() * axis.x() * oneMinusCos + cos,
                      axis.x() * axis.y() * oneMinusCos - axis.z() * sin,
                      axis.x() * axis.z() * oneMinusCos + axis.y() * sin,
                      0.0f,

                      axis.y() * axis.x() * oneMinusCos + axis.z() * sin,
                      axis.y() * axis.y() * oneMinusCos + cos,
                      axis.y() * axis.z() * oneMinusCos - axis.x() * sin,
                      0.0f,

                      axis.z() * axis.x() * oneMinusCos - axis.y() * sin,
                      axis.z() * axis.y() * oneMinusCos + axis.x() * sin,
                      axis.z() * axis.z() * oneMinusCos + cos,
                      0.0f,

                      0.0f,
                      0.0f,
                      0.0f,
                      1.0f};
}

} // namespace

Blending::Blending(Engine const& engine)
  : mGfxCache{engine.create_gfx_resource_cache()}
  , mStarPipeline{[&] {
    auto desc = PipelineCreateInfo{};
    desc.vertexLayout = StarVertex::sLayout;
    desc.dithering = true;

    return mGfxCache->create_pipeline(desc);
  }()}
  , mSampler{[&] {
    auto desc = SamplerCreateInfo{};
    desc.magFilter = TextureFilter::Bilinear;
    desc.minFilter = TextureFilter::Bilinear;
    desc.mipFilter = TextureMipFilter::Linear;

    return mGfxCache->create_sampler(desc);
  }()}
  , mPlanetTextures{[&] {
    auto textures = array<TextureHandle, sNumPlanetTextures>{};
    for (auto i = uSize{0}; i < sNumPlanetTextures; i++) {
      auto const filePath =
        fmt::format(FMT_STRING("data/tribase/02-08_blending/Planet{}.dds"), i);
      textures[i] = mGfxCache->load_texture_2d(filePath);
    }

    return textures;
  }()}
  , mStarsVb{[&] {
    auto starVertices = vector<StarVertex>{NUM_STARS};
    generate_stars(starVertices);
    auto const starVertexData = as_bytes(span{starVertices});

    return mGfxCache->create_vertex_buffer(
      {starVertexData.size_bytes(), StarVertex::sLayout}, starVertexData);
  }()} {
  mPlanetMesh = [&] {
    auto const xModelData = mGfxCache->load_x_meshes(PLANET_MODEL_FILE_PATH);

    return xModelData.meshes.front();
  }();
  mSunMesh = [&] {
    auto const xModelData = mGfxCache->load_x_meshes(SUN_MODEL_FILE_PATH);

    return xModelData.meshes.front();
  }();

  auto vs = FixedVertexShaderCreateInfo{};
  vs.lightingEnabled = true;

  auto fs = FixedFragmentShaderCreateInfo{};
  auto textureStages = array{TextureStage{}};
  std::get<0>(textureStages).alphaOp = TextureOp::Modulate;
  fs.textureStages = textureStages;

  auto planetPipelineDesc = PipelineCreateInfo{};
  planetPipelineDesc.vertexShader = &vs;
  planetPipelineDesc.fragmentShader = &fs;
  planetPipelineDesc.cullMode = CullMode::Clockwise;
  planetPipelineDesc.dithering = true;
  planetPipelineDesc.srcBlendFactor = BlendFactor::SrcAlpha;
  planetPipelineDesc.destBlendFactor = BlendFactor::OneMinusSrcAlpha;
  mPlanetPipelineCw = mGfxCache->create_pipeline(planetPipelineDesc);
  planetPipelineDesc.cullMode = CullMode::CounterClockwise;
  mPlanetPipelineCcw = mGfxCache->create_pipeline(planetPipelineDesc);
}

auto Blending::on_update(UpdateContext& ctx) -> void {
  mTime += ctx.deltaTime;
  auto const t = mTime.count();

  auto const cameraPos =
    Vector3f32{50.0f * std::sin(0.35f * t), 50.0f * std::cos(0.2f * t),
               40.0f * std::sin(0.1f * t) - 10.0f};

  auto planets = array<Planet, 6>{};
  update_planets(planets, t);

  for (auto i = uSize{0}; i < planets.size(); ++i) {
    planets[i].type = static_cast<u8>(i);
    planets[i].distanceToCamera = (planets[i].position - cameraPos).length();
  }

  // sort the transparent planets from back to front
  std::sort(planets.begin(), planets.end(),
            [](Planet const& p1, Planet const& p2) {
              auto const diff = p1.distanceToCamera - p2.distanceToCamera;

              // if p1 is further away from the camera than p2 then render p1
              // before p2
              return diff > 0.0f;
            });

  auto const& drawCtx = ctx.drawCtx;
  auto cmdList = CommandList{};
  cmdList.clear_attachments(Attachments{Attachment::RenderTarget}, BACKGROUND);

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

  cmdList.bind_sampler(0, mSampler);

  constexpr auto lights = array<LightData, 1>{
    PointLightData{Color::from_non_linear(1.0f, 1.0f, 0.75f),
                   {},
                   Color::from_non_linear(1.0f, 1.0f, 0.75f),
                   {},
                   1000,
                   1.0f,
                   0,
                   0}};
  cmdList.set_lights(lights);

  for (auto const& planet : planets) {
    if (planet.type == 0) {
      // this is the sun

      auto randomEngine = default_random_engine{random_device{}()};
      auto rng = Distribution{0.0f, 0.025f};
      for (auto i = i32{0}; i < 21; i++) {
        auto const localToWorld =
          Matrix4x4f32::rotation_x(Angle::radians(
            (static_cast<f32>(i - 10) + rng(randomEngine)) * t / 20.0f)) *
          Matrix4x4f32::rotation_y(Angle::radians(
            (static_cast<f32>(-i - 10) + rng(randomEngine)) * t / 20.0f)) *
          Matrix4x4f32::rotation_z(Angle::radians(
            (static_cast<f32>(i - 10) + rng(randomEngine)) * t / 20.0f)) *
          Matrix4x4f32::scaling(
            Vector3f32{planet.radius + static_cast<f32>(i) / 10.0f} +
            Vector3f32{0.25f * std::sin(4.0f * t)});
        cmdList.set_transform(TransformState::LocalToWorld, localToWorld);
        cmdList.set_material(
          Color::from_non_linear(0, 0, 0, 1.1f - static_cast<f32>(i) / 20.0f),
          {}, Color::from_non_linear(1.0f, 1.0f, 0.75f));

        cmdList.bind_texture(0, std::get<0>(mPlanetTextures));

        cmdList.bind_pipeline(mPlanetPipelineCw);
        XMeshCommandEncoder::draw_x_mesh(cmdList, mSunMesh);
        cmdList.bind_pipeline(mPlanetPipelineCcw);
        XMeshCommandEncoder::draw_x_mesh(cmdList, mSunMesh);
      }

      continue;
    }

    auto const localToWorld =
      Matrix4x4f32::scaling(planet.radius) *
      rotation_axis({0.5f, 1.0f, 0.25f}, Angle::radians(t)) *
      Matrix4x4f32::translation(planet.position);
    cmdList.set_transform(TransformState::LocalToWorld, localToWorld);
    cmdList.set_material(Colors::WHITE,
                         Color::from_non_linear(0.25f, 0.25f, 0.25f));
    cmdList.bind_texture(0, mPlanetTextures[planet.type]);

    cmdList.bind_pipeline(mPlanetPipelineCw);
    XMeshCommandEncoder::draw_x_mesh(cmdList, mPlanetMesh);
    cmdList.bind_pipeline(mPlanetPipelineCcw);
    XMeshCommandEncoder::draw_x_mesh(cmdList, mPlanetMesh);
  }

  drawCtx.commandLists.emplace_back(std::move(cmdList));
}

} // namespace tribase
