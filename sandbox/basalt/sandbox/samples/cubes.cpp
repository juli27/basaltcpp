#include <basalt/sandbox/samples/cubes.h>

#include <basalt/api/engine.h>
#include <basalt/api/input.h>
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

#include <entt/core/hashed_string.hpp>
#include <gsl/span>

#include <array>
#include <random>
#include <string>
#include <string_view>
#include <vector>

namespace samples {

using namespace std::literals;
using std::array;
using std::default_random_engine;
using std::random_device;
using std::uniform_real_distribution;
using std::vector;

using namespace entt::literals;
using gsl::span;

using namespace basalt::literals;
using basalt::Angle;
using basalt::EntityId;
using basalt::EntityName;
using basalt::InputState;
using basalt::Key;
using basalt::Scene;
using basalt::SceneView;
using basalt::System;
using basalt::Vector3f32;
using basalt::gfx::Camera;
using basalt::gfx::CameraEntity;
using basalt::gfx::Environment;
using basalt::gfx::FixedFragmentShaderCreateInfo;
using basalt::gfx::FixedVertexShaderCreateInfo;
using basalt::gfx::Light;
using basalt::gfx::MaterialColorSource;
using basalt::gfx::MaterialDescriptor;
using basalt::gfx::PipelineDescriptor;
using basalt::gfx::PrimitiveType;
using basalt::gfx::RenderComponent;
using basalt::gfx::SpotLight;
using basalt::gfx::TestPassCond;
using basalt::gfx::TextureFilter;
using basalt::gfx::TextureMipFilter;
using basalt::gfx::TextureStage;
using basalt::gfx::VertexElement;

namespace {

using Distribution = uniform_real_distribution<float>;

struct Vertex final {
  Vector3f32 pos;
  Vector3f32 normal;
  ColorEncoding::A8R8G8B8 diffuse {};
  array<f32, 2> uv {};

  static constexpr array sLayout {
    VertexElement::Position3F32, VertexElement::Normal3F32,
    VertexElement::ColorDiffuse1U32A8R8G8B8, VertexElement::TextureCoords2F32};
};

constexpr u32 NUM_CUBES {2048};
constexpr u32 NUM_TRIANGLES_PER_CUBE {2 * 6};
constexpr u32 NUM_VERTICES_PER_CUBE {4 * 6};
constexpr u32 NUM_INDICES_PER_CUBE {NUM_TRIANGLES_PER_CUBE * 3};

static_assert(NUM_CUBES * NUM_VERTICES_PER_CUBE <= 0xffff,
              "can't index vertices with u16");

auto generate_mesh(const span<Vertex> vb, const span<u16> ib) -> void {
  default_random_engine randomEngine {random_device {}()};
  Distribution rng1 {-1.0f, 1.0f};
  Distribution rng2 {20.0f, 250.f};
  Distribution rng3 {0.0f, 1.0f};

  const auto normalizedRandomVector {[&] {
    return Vector3f32::normalize(rng1(randomEngine), rng1(randomEngine),
                                 rng1(randomEngine));
  }};

  constexpr array<u16, NUM_INDICES_PER_CUBE> cubeIndices {
    8,  9,  11, 8,  11, 10, // front
    13, 12, 14, 13, 14, 15, // back
    17, 16, 18, 17, 18, 19, // left
    21, 20, 22, 21, 22, 23, // right
    0,  1,  2,  0,  2,  3, // top
    6,  5,  4,  6,  4,  7}; // bottom

  for (u32 iCube {0}; iCube < NUM_CUBES; iCube++) {
    const Vector3f32 pos {normalizedRandomVector() * rng2(randomEngine)};

    const u16 startVertex {static_cast<u16>(NUM_VERTICES_PER_CUBE * iCube)};

    // top
    vb[startVertex + 0].pos = pos + Vector3f32 {-1.0f, 1.0f, -1.0f};
    vb[startVertex + 0].normal = {0, 1.0f, 0};
    vb[startVertex + 1].pos = pos + Vector3f32 {-1.0f, 1.0f, 1.0f};
    vb[startVertex + 1].normal = {0, 1.0f, 0};
    vb[startVertex + 2].pos = pos + Vector3f32 {1.0f, 1.0f, 1.0f};
    vb[startVertex + 2].normal = {0, 1.0f, 0};
    vb[startVertex + 3].pos = pos + Vector3f32 {1.0f, 1.0f, -1.0f};
    vb[startVertex + 3].normal = {0, 1.0f, 0};

    // bottom
    vb[startVertex + 4].pos = pos + Vector3f32 {-1.0f, -1.0f, -1.0f};
    vb[startVertex + 4].normal = {0, -1.0f, 0};
    vb[startVertex + 5].pos = pos + Vector3f32 {-1.0f, -1.0f, 1.0f};
    vb[startVertex + 5].normal = {0, -1.0f, 0};
    vb[startVertex + 6].pos = pos + Vector3f32 {1.0f, -1.0f, 1.0f};
    vb[startVertex + 6].normal = {0, -1.0f, 0};
    vb[startVertex + 7].pos = pos + Vector3f32 {1.0f, -1.0f, -1.0f};
    vb[startVertex + 7].normal = {0, -1.0f, 0};

    for (Vertex& vertex : vb.subspan(startVertex, 8)) {
      const Color vertexColor {Color::from_non_linear(rng3(randomEngine),
                                                      rng3(randomEngine),
                                                      rng3(randomEngine)) *
                               2.0f};
      vertex.diffuse = vertexColor.to_argb();

      vertex.uv = {rng1(randomEngine), rng1(randomEngine)};
    }

    // duplicate vertices for each face of the cube for correct lighting
    // (normals)
    // front
    vb[startVertex + 8] = vb[startVertex + 0];
    vb[startVertex + 8].normal = Vector3f32 {0, 0, -1.0f};
    vb[startVertex + 9] = vb[startVertex + 3];
    vb[startVertex + 9].normal = Vector3f32 {0, 0, -1.0f};
    vb[startVertex + 10] = vb[startVertex + 4];
    vb[startVertex + 10].normal = Vector3f32 {0, 0, -1.0f};
    vb[startVertex + 11] = vb[startVertex + 7];
    vb[startVertex + 11].normal = Vector3f32 {0, 0, -1.0f};
    // back
    vb[startVertex + 12] = vb[startVertex + 1];
    vb[startVertex + 12].normal = Vector3f32 {0, 0, 1.0f};
    vb[startVertex + 13] = vb[startVertex + 2];
    vb[startVertex + 13].normal = Vector3f32 {0, 0, 1.0f};
    vb[startVertex + 14] = vb[startVertex + 5];
    vb[startVertex + 14].normal = Vector3f32 {0, 0, 1.0f};
    vb[startVertex + 15] = vb[startVertex + 6];
    vb[startVertex + 15].normal = Vector3f32 {0, 0, 1.0f};
    // left
    vb[startVertex + 16] = vb[startVertex + 0];
    vb[startVertex + 16].normal = Vector3f32 {-1.0f, 0, 0};
    vb[startVertex + 17] = vb[startVertex + 1];
    vb[startVertex + 17].normal = Vector3f32 {-1.0f, 0, 0};
    vb[startVertex + 18] = vb[startVertex + 4];
    vb[startVertex + 18].normal = Vector3f32 {-1.0f, 0, 0};
    vb[startVertex + 19] = vb[startVertex + 5];
    vb[startVertex + 19].normal = Vector3f32 {-1.0f, 0, 0};
    // right
    vb[startVertex + 20] = vb[startVertex + 2];
    vb[startVertex + 20].normal = Vector3f32 {1.0f, 0, 0};
    vb[startVertex + 21] = vb[startVertex + 3];
    vb[startVertex + 21].normal = Vector3f32 {1.0f, 0, 0};
    vb[startVertex + 22] = vb[startVertex + 6];
    vb[startVertex + 22].normal = Vector3f32 {1.0f, 0, 0};
    vb[startVertex + 23] = vb[startVertex + 7];
    vb[startVertex + 23].normal = Vector3f32 {1.0f, 0, 0};

    const u32 startIndex {NUM_INDICES_PER_CUBE * iCube};

    for (u32 iIndex {0}; iIndex < NUM_INDICES_PER_CUBE; iIndex++) {
      // add startIndex as offset to every index
      ib[startIndex + iIndex] = cubeIndices[iIndex] + startVertex;
    }
  }
}

constexpr entt::id_type CONTROLLED_CAMERA {"controlled camera"_hs};

struct CameraControllerData final {
  Angle angleY;
};

class RotatingLightSystem final : public System {
public:
  using UpdateBefore = basalt::TransformSystem;

  auto on_update(const UpdateContext& ctx) -> void override {
    ctx.scene.entity_registry().view<Light>().each([&](Light& light) {
      const f32 lightAngle {ctx.time.count() * 0.1f};

      std::get<SpotLight>(light).direction =
        Vector3f32 {std::cos(lightAngle), 0, std::sin(lightAngle)}.normalize();
    });
  }

  RotatingLightSystem() noexcept = default;
};

class CameraController final : public System {
public:
  using UpdateBefore = basalt::TransformSystem;

  auto on_update(const UpdateContext& ctx) -> void override {
    auto& scene {ctx.scene};
    const auto& entities {scene.entity_registry()};
    const auto& input {entities.ctx().get<const InputState>()};

    const CameraEntity camera {
      scene.get_handle(entities.ctx().get<EntityId>(CONTROLLED_CAMERA))};
    auto& transform {camera.get_transform()};
    auto& cameraData {camera.get_camera()};
    auto& controllerData {camera.entity().get<CameraControllerData>()};
    const f32 dt {ctx.deltaTime.count()};

    if (input.is_key_down(Key::LeftArrow)) {
      controllerData.angleY -= Angle::degrees(45 * dt);
    }
    if (input.is_key_down(Key::RightArrow)) {
      controllerData.angleY += Angle::degrees(45 * dt);
    }

    const Vector3f32 direction {controllerData.angleY.sin(), 0,
                                controllerData.angleY.cos()};

    if (input.is_key_down(Key::UpArrow)) {
      transform.position += direction * (10 * dt);
    }
    if (input.is_key_down(Key::DownArrow)) {
      transform.position -= direction * (10 * dt);
    }

    cameraData.lookAt = transform.position + direction;

    Angle fov {cameraData.fov};

    if (input.is_key_down(Key::NumpadSub) || input.is_key_down(Key::Minus)) {
      fov -= Angle::degrees(15 * dt);
    }
    if (input.is_key_down(Key::NumpadAdd) || input.is_key_down(Key::Plus)) {
      fov += Angle::degrees(15 * dt);
    }

    if (fov.degrees() <= 0) {
      fov = 0.1_deg;
    }
    if (fov.degrees() >= 179) {
      fov = 179_deg;
    }

    cameraData.fov = fov;
  }

  CameraController() noexcept = default;
};

} // namespace

Cubes::Cubes(basalt::Engine& engine)
  : mGfxCache {engine.create_gfx_resource_cache()} {
  constexpr u32 vertexCount {NUM_VERTICES_PER_CUBE * NUM_CUBES};
  constexpr u32 indexCount {NUM_INDICES_PER_CUBE * NUM_CUBES};
  vector<Vertex> vertices {vertexCount};
  vector<u16> indices(indexCount);
  generate_mesh(vertices, indices);

  const auto vertexData {as_bytes(span {vertices})};
  const auto indexData {as_bytes(span {indices})};
  const auto mesh {mGfxCache->create_mesh(
    {vertexData, vertexCount, Vertex::sLayout, indexData, indexCount})};

  FixedVertexShaderCreateInfo vs;
  vs.lightingEnabled = true;
  vs.ambientSource = MaterialColorSource::DiffuseVertexColor;

  FixedFragmentShaderCreateInfo fs;
  array textureStages {TextureStage {}};
  fs.textureStages = textureStages;

  PipelineDescriptor pipelineDesc;
  pipelineDesc.vertexShader = &vs;
  pipelineDesc.fragmentShader = &fs;
  pipelineDesc.vertexLayout = Vertex::sLayout;
  pipelineDesc.primitiveType = PrimitiveType::TriangleList;
  pipelineDesc.depthTest = TestPassCond::IfLessEqual;
  pipelineDesc.depthWriteEnable = true;
  MaterialDescriptor matDesc;
  matDesc.pipelineDesc = &pipelineDesc;
  matDesc.sampledTexture.texture =
    mGfxCache->load_texture("data/tribase/Texture2.bmp"sv);
  matDesc.sampledTexture.filter = TextureFilter::Bilinear;
  matDesc.sampledTexture.mipFilter = TextureMipFilter::Linear;
  const auto material {mGfxCache->create_material(matDesc)};

  const auto scene {Scene::create()};
  scene->create_system<RotatingLightSystem>();
  scene->create_system<CameraController>();
  auto& gfxEnv {scene->entity_registry().ctx().emplace<Environment>()};
  gfxEnv.set_background(Colors::BLACK);
  gfxEnv.set_ambient_light(Color::from_non_linear(0.2f, 0.2f, 0.2f));
  auto& entities {scene->entity_registry()};

  const auto cubes {scene->create_entity()};
  cubes.emplace<EntityName>("Cubes"s);
  cubes.emplace<RenderComponent>(mesh, material);

  const auto light {scene->create_entity()};
  light.emplace<EntityName>("SpotLight"s);
  light.emplace<Light>(SpotLight {Colors::WHITE,
                                  {},
                                  {},
                                  Vector3f32::forward(),
                                  250.0f,
                                  0,
                                  0.025f,
                                  0,
                                  1.0f,
                                  45_deg,
                                  15_deg});

  const auto camera {scene->create_entity()};
  camera.emplace<EntityName>("Camera"s);
  camera.emplace<Camera>(Vector3f32::forward(), Vector3f32::up(), 90_deg, 0.1f,
                         250.0f);
  camera.emplace<CameraControllerData>(CameraControllerData {0_deg});

  entities.ctx().emplace_as<EntityId>(CONTROLLED_CAMERA, camera.entity());

  add_child_bottom(SceneView::create(scene, mGfxCache, camera.entity()));
}

} // namespace samples
