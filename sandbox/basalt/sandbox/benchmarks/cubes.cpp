#include "benchmarks.h"

#include <basalt/sandbox/shared/debug_scene_view.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/camera.h>
#include <basalt/api/gfx/environment.h>
#include <basalt/api/gfx/material.h>
#include <basalt/api/gfx/material_class.h>
#include <basalt/api/gfx/mesh.h>
#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/backend/buffer.h>
#include <basalt/api/gfx/backend/vertex_layout.h>

#include <basalt/api/scene/scene.h>
#include <basalt/api/scene/system.h>
#include <basalt/api/scene/transform.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/vector2.h>
#include <basalt/api/math/vector3.h>

#include <fmt/format.h>
#include <gsl/span>
#include <imgui.h>

#include <algorithm>
#include <array>
#include <random>
#include <utility>
#include <vector>

using namespace basalt;
using namespace std::literals;

namespace {

using Distribution = std::uniform_real_distribution<float>;

struct Vertex {
  Vector3f32 pos;
  ColorEncoding::A8R8G8B8 diffuse{};
  Vector2f32 uv;

  static auto constexpr sLayout =
    gfx::make_vertex_layout<gfx::VertexElement::Position3F32,
                            gfx::VertexElement::ColorDiffuse1U32A8R8G8B8,
                            gfx::VertexElement::TextureCoords2F32>();
};

auto constexpr TEXTURE_FILE_PATH = "data/tribase/Texture2.bmp"sv;

auto constexpr INITIAL_NUM_CUBES = u32{2048};
auto constexpr NUM_TRIANGLES_PER_CUBE = u32{2 * 6};
auto constexpr NUM_VERTICES_PER_CUBE = u32{8};
auto constexpr NUM_INDICES_PER_CUBE = u32{NUM_TRIANGLES_PER_CUBE * 3};

auto generate_mesh(gsl::span<Vertex> const vb, gsl::span<u16> const ib)
  -> void {
  auto randomEngine = std::default_random_engine{std::random_device{}()};
  auto rng1 = Distribution{-1.0f, 1.0f};
  auto rng2 = Distribution{0.0f, 1.0f};

  auto constexpr cubePositions = std::array{
    Vector3f32{-1.0f, 1.0f, -1.0f},  Vector3f32{-1.0f, 1.0f, 1.0f},
    Vector3f32{1.0f, 1.0f, 1.0f},    Vector3f32{1.0f, 1.0f, -1.0f},
    Vector3f32{-1.0f, -1.0f, -1.0f}, Vector3f32{-1.0f, -1.0f, 1.0f},
    Vector3f32{1.0f, -1.0f, 1.0f},   Vector3f32{1.0f, -1.0f, -1.0f},
  };

  for (auto i = uSize{0}; i < NUM_VERTICES_PER_CUBE; i++) {
    auto const vertexColor =
      2.0f * Color::from_non_linear(rng2(randomEngine), rng2(randomEngine),
                                    rng2(randomEngine));
    vb[i] = {
      cubePositions[i],
      vertexColor.to_argb(),
      Vector2f32{rng1(randomEngine), rng1(randomEngine)},
    };
  }

  auto constexpr cubeIndices =
    std::array<u16, NUM_INDICES_PER_CUBE>{0, 3, 7, 0, 7, 4, // front
                                          2, 1, 5, 2, 5, 6, // back
                                          1, 0, 4, 1, 4, 5, // left
                                          3, 2, 6, 3, 6, 7, // right
                                          0, 1, 2, 0, 2, 3, // top
                                          6, 5, 4, 6, 4, 7}; // bottom

  std::copy(cubeIndices.begin(), cubeIndices.end(), ib.begin());
}

struct VelocityComponent {
  Vector3f32 value;
};

class VelocitySystem final : public System {
public:
  auto on_update(UpdateContext const& ctx) -> void override {
    auto const dt = ctx.deltaTime.count();

    auto const view =
      ctx.scene.entity_registry().view<VelocityComponent, Transform>();
    view.each([&](VelocityComponent& velocity, Transform& transform) {
      transform.position += velocity.value * dt;

      if (transform.position.length() > 250.0f) {
        velocity.value.negate();
      }
    });
  }
};

class CubesView final : public View {
public:
  CubesView(SceneViewPtr sceneView, SystemId const velocitySystemId,
            gfx::MaterialHandle const materialHdl,
            gfx::MeshHandle const meshHdl)
    : mScene{sceneView->scene()}
    , mVelocitySystemId{velocitySystemId}
    , mMaterialHdl{materialHdl}
    , mMeshHdl{meshHdl}
    , mCameraEntityId{sceneView->camera_entity_id()} {
    add_child_bottom(std::move(sceneView));
    regenerate_cubes();
  }

private:
  ScenePtr mScene;
  std::vector<EntityId> mCubeIds;
  SystemId mVelocitySystemId;
  gfx::MaterialHandle mMaterialHdl;
  gfx::MeshHandle mMeshHdl;
  u32 mNumCubes{INITIAL_NUM_CUBES};
  EntityId mCameraEntityId;
  Angle mCameraAngleY{0_deg};
  bool mDoCubesFollowCamera{false};

  auto regenerate_cubes() -> void {
    mScene->entity_registry().destroy(mCubeIds.cbegin(), mCubeIds.cend());
    mCubeIds.clear();

    auto randomEngine = std::default_random_engine{std::random_device{}()};
    auto rng1 = Distribution{-1.0f, 1.0f};
    auto rng2 = Distribution{20.0f, 250.f};
    auto rng3 = Distribution{0.1f, 5.0f};

    auto const normalizedRandomVector = [&] {
      return Vector3f32::normalized(rng1(randomEngine), rng1(randomEngine),
                                    rng1(randomEngine));
    };

    mCubeIds.reserve(mNumCubes);
    for (auto i = u32{0}; i < mNumCubes; ++i) {
      auto const cubePos = normalizedRandomVector() * rng2(randomEngine);
      auto cube = mScene->create_entity(
        fmt::format(FMT_STRING("Cube {}"), i + 1), cubePos);
      cube.emplace<VelocityComponent>(normalizedRandomVector() *
                                      rng3(randomEngine));
      cube.emplace<gfx::Model>(mMeshHdl, mMaterialHdl);

      mCubeIds.push_back(cube.entity());
    }
  }

  auto regenerate_velocities() -> void {
    auto randomEngine = std::default_random_engine{std::random_device{}()};
    auto rng1 = Distribution{-1.0f, 1.0f};
    auto rng2 = Distribution{0.1f, 5.0f};

    auto const normalizedRandomVector = [&] {
      return Vector3f32::normalized(rng1(randomEngine), rng1(randomEngine),
                                    rng1(randomEngine));
    };

    auto const velocityComponents =
      mScene->entity_registry().view<VelocityComponent>();
    velocityComponents.each([&](VelocityComponent& velocity) {
      velocity.value = normalizedRandomVector() * rng2(randomEngine);
    });
  }

  auto on_update(UpdateContext& ctx) -> void override {
    if (ImGui::IsKeyPressed(ImGuiKey_Space, false)) {
      if (mVelocitySystemId) {
        mScene->destroy_system(mVelocitySystemId);
        mVelocitySystemId = nullhdl;
      } else {
        mVelocitySystemId = mScene->create_system<VelocitySystem>();
      }
    }

    if (ImGui::Begin("Settings##TribaseBuffersEx")) {
      ImGui::TextUnformatted(mVelocitySystemId ? "Press SPACE to pause"
                                               : "Press SPACE to unpause");

      {
        ImGui::SeparatorText("Number of Cubes");

        auto numCubes = static_cast<int>(mNumCubes);
        ImGui::DragInt("##numCubes", &numCubes, 4.0f, 0, 1 << 24, "%d",
                       ImGuiSliderFlags_AlwaysClamp);
        mNumCubes = static_cast<uSize>(numCubes);

        if (ImGui::Button("Apply")) {
          regenerate_cubes();
        }
      }

      ImGui::Separator();

      if (ImGui::Checkbox("Cubes follow camera", &mDoCubesFollowCamera)) {
        if (!mDoCubesFollowCamera) {
          regenerate_velocities();
        }
      }
    }
    ImGui::End();

    auto const dt = ctx.deltaTime.count();
    auto const camera = gfx::CameraEntity{mScene->get_handle(mCameraEntityId)};

    if (is_key_down(Key::A) || is_key_down(Key::LeftArrow)) {
      mCameraAngleY -= Angle::degrees(45.0f * dt);
    }
    if (is_key_down(Key::D) || is_key_down(Key::RightArrow)) {
      mCameraAngleY += Angle::degrees(45.0f * dt);
    }

    auto const dir = Vector3f32{mCameraAngleY.sin(), 0.0f, mCameraAngleY.cos()};
    auto& cameraTransform = camera.get_transform();
    if (is_key_down(Key::W) || is_key_down(Key::UpArrow)) {
      cameraTransform.position += dir * (10.0f * dt);
    }
    if (is_key_down(Key::S) || is_key_down(Key::DownArrow)) {
      cameraTransform.position -= dir * (10.0f * dt);
    }

    auto& cameraComponent = camera.get_camera();
    cameraComponent.lookAt =
      cameraTransform.position +
      Vector3f32{mCameraAngleY.sin(), 0.0f, mCameraAngleY.cos()};

    if (is_key_down(Key::Q)) {
      cameraComponent.fov -= Angle::degrees(15.f * dt);
    }
    if (is_key_down(Key::E)) {
      cameraComponent.fov += Angle::degrees(15.f * dt);
    }

    if (cameraComponent.fov.degrees() <= 0) {
      cameraComponent.fov = 0.1_deg;
    }
    if (cameraComponent.fov.degrees() >= 179) {
      cameraComponent.fov = 179_deg;
    }

    if (mDoCubesFollowCamera) {
      auto const velocityComponents =
        mScene->entity_registry().view<VelocityComponent, Transform const>();
      velocityComponents.each(
        [&](VelocityComponent& velocity, Transform const& transform) {
          auto const toCamera = cameraTransform.position - transform.position;
          velocity.value += 0.01f * dt * toCamera;
        });
    }
  }

  auto on_input(InputEvent const&) -> InputEventHandled override {
    return InputEventHandled::Yes;
  }
};

struct CubeMeshData {
  static auto generate() -> CubeMeshData {
    auto vertices = std::vector<Vertex>(NUM_VERTICES_PER_CUBE);
    auto indices = std::vector<u16>(NUM_INDICES_PER_CUBE);
    generate_mesh(vertices, indices);

    return CubeMeshData{std::move(vertices), std::move(indices)};
  }

  std::vector<Vertex> vertices;
  std::vector<u16> indices;
};

// TODO: exercise 3 + demo
// auto transform_vertex_positions(const span<byte> vertexData,
//                                const VertexLayout layout,
//                                const Matrix4x4f32& transform) -> void {
//}

} // namespace

auto Benchmarks::make_cubes_view(Engine& engine) -> ViewPtr {
  auto sceneResources = engine.create_gfx_resource_cache();
  auto const materialHdl = [&] {
    auto info = gfx::MaterialCreateInfo{};
    info.clazz = [&] {
      auto info = gfx::MaterialClassCreateInfo{};
      auto& pipelineInfo = info.pipelineInfo;

      auto fs = gfx::FixedFragmentShaderCreateInfo{};
      auto constexpr textureStages = std::array{gfx::TextureStage{}};
      fs.textureStages = textureStages;
      pipelineInfo.fragmentShader = &fs;

      pipelineInfo.vertexLayout = Vertex::sLayout;
      pipelineInfo.primitiveType = gfx::PrimitiveType::TriangleList;
      pipelineInfo.depthTest = gfx::TestPassCond::IfLessEqual;
      pipelineInfo.depthWriteEnable = true;

      return sceneResources->create_material_class(info);
    }();

    auto const samplerHdl = [&] {
      auto info = gfx::SamplerCreateInfo{};
      info.magFilter = gfx::TextureFilter::Bilinear;
      info.minFilter = gfx::TextureFilter::Bilinear;
      info.mipFilter = gfx::TextureMipFilter::Linear;

      return sceneResources->create_sampler(info);
    }();
    auto const textureHdl = sceneResources->load_texture_2d(TEXTURE_FILE_PATH);
    auto const properties = std::array{
      gfx::MaterialProperty{gfx::MaterialPropertyId::SampledTexture,
                            gfx::SampledTexture{samplerHdl, textureHdl}},
    };
    info.initialValues = properties;

    return sceneResources->create_material(info);
  }();

  auto const meshHdl = [&] {
    auto const meshData = CubeMeshData::generate();

    auto info = gfx::MeshCreateInfo{};
    info.vertexBuffer = [&] {
      auto const vertexData = as_bytes(gsl::span{meshData.vertices});

      auto info = gfx::VertexBufferCreateInfo{};
      info.sizeInBytes = vertexData.size_bytes();
      info.layout = Vertex::sLayout;

      return sceneResources->create_vertex_buffer(info, vertexData);
    }();
    info.vertexCount = NUM_VERTICES_PER_CUBE;
    info.indexBuffer = [&] {
      auto const indexData = as_bytes(gsl::span{meshData.indices});

      auto info = gfx::IndexBufferCreateInfo{};
      info.sizeInBytes = indexData.size_bytes();

      return sceneResources->create_index_buffer(info, indexData);
    }();
    info.indexCount = NUM_INDICES_PER_CUBE;

    return sceneResources->create_mesh(info);
  }();

  auto scene = Scene::create();
  auto const velocitySystemId = scene->create_system<VelocitySystem>();
  auto& gfxEnv = scene->entity_registry().ctx().emplace<gfx::Environment>();
  gfxEnv.set_background(Colors::BLACK);

  auto camera = scene->create_entity("Camera"s);
  camera.emplace<gfx::Camera>(Vector3f32::forward(), Vector3f32::up(), 90_deg,
                              0.1f, 250.0f);

  auto sceneView = DebugSceneView::create(
    std::move(scene), std::move(sceneResources), camera.entity());

  return std::make_shared<CubesView>(std::move(sceneView), velocitySystemId,
                                     materialHdl, meshHdl);
}
