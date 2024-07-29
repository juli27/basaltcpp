#include <basalt/sandbox/samples/samples.h>

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
#include <basalt/api/math/vector2.h>
#include <basalt/api/math/vector3.h>

#include <entt/core/hashed_string.hpp>
#include <gsl/span>

#include <array>
#include <random>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

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
using basalt::Engine;
using basalt::EntityId;
using basalt::InputState;
using basalt::Key;
using basalt::Scene;
using basalt::SceneView;
using basalt::System;
using basalt::Vector2f32;
using basalt::Vector3f32;
using basalt::ViewPtr;
using basalt::gfx::Camera;
using basalt::gfx::CameraEntity;
using basalt::gfx::Environment;
using basalt::gfx::FixedFragmentShaderCreateInfo;
using basalt::gfx::FixedVertexShaderCreateInfo;
using basalt::gfx::Light;
using basalt::gfx::MaterialColorSource;
using basalt::gfx::MaterialCreateInfo;
using basalt::gfx::PipelineCreateInfo;
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
  ColorEncoding::A8R8G8B8 diffuse{};
  Vector2f32 uv{};

  static constexpr auto sLayout = array{
    VertexElement::Position3F32,
    VertexElement::Normal3F32,
    VertexElement::ColorDiffuse1U32A8R8G8B8,
    VertexElement::TextureCoords2F32,
  };
};

constexpr auto CUBE_TEXTURE_FILE_PATH = "data/tribase/Texture2.bmp"sv;

constexpr auto NUM_CUBES = u32{2048};
constexpr auto NUM_TRIANGLES_PER_CUBE = u32{2 * 6};
constexpr auto NUM_VERTICES_PER_CUBE = u32{4 * 6};
constexpr auto NUM_INDICES_PER_CUBE = u32{NUM_TRIANGLES_PER_CUBE * 3};

static_assert(NUM_CUBES * NUM_VERTICES_PER_CUBE <= 0xffff,
              "can't index vertices with u16");

auto generate_mesh(span<Vertex> const vb, span<u16> const ib) -> void {
  auto randomEngine = default_random_engine{random_device{}()};
  auto rng1 = Distribution{-1.0f, 1.0f};
  auto rng2 = Distribution{20.0f, 250.f};
  auto rng3 = Distribution{0.0f, 1.0f};

  auto const normalizedRandomVector{[&] {
    return Vector3f32::normalize(rng1(randomEngine), rng1(randomEngine),
                                 rng1(randomEngine));
  }};

  constexpr auto cubeIndices =
    array<u16, NUM_INDICES_PER_CUBE>{8,  9,  11, 8,  11, 10, // front
                                     13, 12, 14, 13, 14, 15, // back
                                     17, 16, 18, 17, 18, 19, // left
                                     21, 20, 22, 21, 22, 23, // right
                                     0,  1,  2,  0,  2,  3, // top
                                     6,  5,  4,  6,  4,  7}; // bottom

  for (auto iCube = u32{0}; iCube < NUM_CUBES; iCube++) {
    auto const pos = Vector3f32{normalizedRandomVector() * rng2(randomEngine)};

    auto const startVertex = static_cast<u16>(NUM_VERTICES_PER_CUBE * iCube);

    // top
    vb[startVertex + 0].pos = pos + Vector3f32{-1.0f, 1.0f, -1.0f};
    vb[startVertex + 0].normal = {0, 1.0f, 0};
    vb[startVertex + 1].pos = pos + Vector3f32{-1.0f, 1.0f, 1.0f};
    vb[startVertex + 1].normal = {0, 1.0f, 0};
    vb[startVertex + 2].pos = pos + Vector3f32{1.0f, 1.0f, 1.0f};
    vb[startVertex + 2].normal = {0, 1.0f, 0};
    vb[startVertex + 3].pos = pos + Vector3f32{1.0f, 1.0f, -1.0f};
    vb[startVertex + 3].normal = {0, 1.0f, 0};

    // bottom
    vb[startVertex + 4].pos = pos + Vector3f32{-1.0f, -1.0f, -1.0f};
    vb[startVertex + 4].normal = {0, -1.0f, 0};
    vb[startVertex + 5].pos = pos + Vector3f32{-1.0f, -1.0f, 1.0f};
    vb[startVertex + 5].normal = {0, -1.0f, 0};
    vb[startVertex + 6].pos = pos + Vector3f32{1.0f, -1.0f, 1.0f};
    vb[startVertex + 6].normal = {0, -1.0f, 0};
    vb[startVertex + 7].pos = pos + Vector3f32{1.0f, -1.0f, -1.0f};
    vb[startVertex + 7].normal = {0, -1.0f, 0};

    for (auto& vertex : vb.subspan(startVertex, 8)) {
      auto const vertexColor =
        2.0f * Color::from_non_linear(rng3(randomEngine), rng3(randomEngine),
                                      rng3(randomEngine));
      vertex.diffuse = vertexColor.to_argb();

      vertex.uv = {rng1(randomEngine), rng1(randomEngine)};
    }

    // duplicate vertices for each face of the cube for correct lighting
    // (normals)
    // front
    vb[startVertex + 8] = vb[startVertex + 0];
    vb[startVertex + 8].normal = Vector3f32{0, 0, -1.0f};
    vb[startVertex + 9] = vb[startVertex + 3];
    vb[startVertex + 9].normal = Vector3f32{0, 0, -1.0f};
    vb[startVertex + 10] = vb[startVertex + 4];
    vb[startVertex + 10].normal = Vector3f32{0, 0, -1.0f};
    vb[startVertex + 11] = vb[startVertex + 7];
    vb[startVertex + 11].normal = Vector3f32{0, 0, -1.0f};
    // back
    vb[startVertex + 12] = vb[startVertex + 1];
    vb[startVertex + 12].normal = Vector3f32{0, 0, 1.0f};
    vb[startVertex + 13] = vb[startVertex + 2];
    vb[startVertex + 13].normal = Vector3f32{0, 0, 1.0f};
    vb[startVertex + 14] = vb[startVertex + 5];
    vb[startVertex + 14].normal = Vector3f32{0, 0, 1.0f};
    vb[startVertex + 15] = vb[startVertex + 6];
    vb[startVertex + 15].normal = Vector3f32{0, 0, 1.0f};
    // left
    vb[startVertex + 16] = vb[startVertex + 0];
    vb[startVertex + 16].normal = Vector3f32{-1.0f, 0, 0};
    vb[startVertex + 17] = vb[startVertex + 1];
    vb[startVertex + 17].normal = Vector3f32{-1.0f, 0, 0};
    vb[startVertex + 18] = vb[startVertex + 4];
    vb[startVertex + 18].normal = Vector3f32{-1.0f, 0, 0};
    vb[startVertex + 19] = vb[startVertex + 5];
    vb[startVertex + 19].normal = Vector3f32{-1.0f, 0, 0};
    // right
    vb[startVertex + 20] = vb[startVertex + 2];
    vb[startVertex + 20].normal = Vector3f32{1.0f, 0, 0};
    vb[startVertex + 21] = vb[startVertex + 3];
    vb[startVertex + 21].normal = Vector3f32{1.0f, 0, 0};
    vb[startVertex + 22] = vb[startVertex + 6];
    vb[startVertex + 22].normal = Vector3f32{1.0f, 0, 0};
    vb[startVertex + 23] = vb[startVertex + 7];
    vb[startVertex + 23].normal = Vector3f32{1.0f, 0, 0};

    auto const startIndex = NUM_INDICES_PER_CUBE * iCube;

    for (auto iIndex = u32{0}; iIndex < NUM_INDICES_PER_CUBE; iIndex++) {
      // add startIndex as offset to every index
      ib[startIndex + iIndex] = cubeIndices[iIndex] + startVertex;
    }
  }
}

constexpr auto CONTROLLED_CAMERA = "controlled camera"_hs.value();

struct CameraControllerData final {
  Angle angleY;
};

class RotatingLightSystem final : public System {
public:
  using UpdateBefore = basalt::TransformSystem;

  auto on_update(UpdateContext const& ctx) -> void override {
    ctx.scene.entity_registry().view<Light>().each([&](Light& light) {
      auto const lightAngle = 0.1f * ctx.time.count();

      std::get<SpotLight>(light).direction =
        Vector3f32{std::cos(lightAngle), 0, std::sin(lightAngle)}.normalize();
    });
  }

  RotatingLightSystem() noexcept = default;
};

class CameraController final : public System {
public:
  using UpdateBefore = basalt::TransformSystem;

  auto on_update(UpdateContext const& ctx) -> void override {
    auto& scene = ctx.scene;
    auto const& entities = scene.entity_registry();
    auto const& input = entities.ctx().get<InputState const>();

    auto const camera = CameraEntity{
      scene.get_handle(entities.ctx().get<EntityId>(CONTROLLED_CAMERA))};
    auto& transform = camera.get_transform();
    auto& cameraData = camera.get_camera();
    auto& controllerData = camera.entity().get<CameraControllerData>();
    auto const dt = ctx.deltaTime.count();

    if (input.is_key_down(Key::LeftArrow)) {
      controllerData.angleY -= Angle::degrees(45 * dt);
    }
    if (input.is_key_down(Key::RightArrow)) {
      controllerData.angleY += Angle::degrees(45 * dt);
    }

    auto const direction =
      Vector3f32{controllerData.angleY.sin(), 0, controllerData.angleY.cos()};

    if (input.is_key_down(Key::UpArrow)) {
      transform.position += direction * (10 * dt);
    }
    if (input.is_key_down(Key::DownArrow)) {
      transform.position -= direction * (10 * dt);
    }

    cameraData.lookAt = transform.position + direction;

    auto fov = cameraData.fov;

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

auto Samples::new_cubes_sample(Engine& engine) -> ViewPtr {
  auto gfxCache = engine.create_gfx_resource_cache();

  constexpr auto vertexCount = NUM_VERTICES_PER_CUBE * NUM_CUBES;
  constexpr auto indexCount = NUM_INDICES_PER_CUBE * NUM_CUBES;
  auto vertices = vector<Vertex>{vertexCount};
  auto indices = vector<u16>(indexCount);
  generate_mesh(vertices, indices);

  auto const vertexData = as_bytes(span{vertices});
  auto const indexData = as_bytes(span{indices});
  auto const mesh = gfxCache->create_mesh(
    {vertexData, vertexCount, Vertex::sLayout, indexData, indexCount});

  auto matDesc = MaterialCreateInfo{};
  matDesc.pipeline = [&] {
    auto vs = FixedVertexShaderCreateInfo{};
    vs.lightingEnabled = true;
    vs.ambientSource = MaterialColorSource::DiffuseVertexColor;

    auto fs = FixedFragmentShaderCreateInfo{};
    constexpr auto textureStages = array{TextureStage{}};
    fs.textureStages = textureStages;

    auto pipelineDesc = PipelineCreateInfo{};
    pipelineDesc.vertexShader = &vs;
    pipelineDesc.fragmentShader = &fs;
    pipelineDesc.vertexLayout = Vertex::sLayout;
    pipelineDesc.primitiveType = PrimitiveType::TriangleList;
    pipelineDesc.depthTest = TestPassCond::IfLessEqual;
    pipelineDesc.depthWriteEnable = true;

    return gfxCache->create_pipeline(pipelineDesc);
  }();
  matDesc.sampledTexture.texture =
    gfxCache->load_texture_2d(CUBE_TEXTURE_FILE_PATH);
  matDesc.sampledTexture.sampler =
    gfxCache->create_sampler({TextureFilter::Bilinear, TextureFilter::Bilinear,
                              TextureMipFilter::Linear});
  auto const material = gfxCache->create_material(matDesc);

  auto scene = Scene::create();
  scene->create_system<RotatingLightSystem>();
  scene->create_system<CameraController>();
  auto& gfxEnv = scene->entity_registry().ctx().emplace<Environment>();
  gfxEnv.set_background(Colors::BLACK);
  gfxEnv.set_ambient_light(Color::from_non_linear(0.2f, 0.2f, 0.2f));
  auto& entities = scene->entity_registry();

  auto const cubes = scene->create_entity("Cubes"s);
  cubes.emplace<RenderComponent>(mesh, material);

  auto const light = scene->create_entity("SpotLight"s);
  light.emplace<Light>(SpotLight{Colors::WHITE,
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

  auto const camera = scene->create_entity("Camera"s);
  camera.emplace<Camera>(Vector3f32::forward(), Vector3f32::up(), 90_deg, 0.1f,
                         250.0f);
  camera.emplace<CameraControllerData>(CameraControllerData{0_deg});

  entities.ctx().emplace_as<EntityId>(CONTROLLED_CAMERA, camera.entity());

  return SceneView::create(std::move(scene), std::move(gfxCache),
                           camera.entity());
}
