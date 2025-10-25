#include "benchmarks.h"

#include <basalt/sandbox/shared/debug_scene_view.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/camera.h>
#include <basalt/api/gfx/context.h>
#include <basalt/api/gfx/environment.h>
#include <basalt/api/gfx/material.h>
#include <basalt/api/gfx/mesh.h>
#include <basalt/api/gfx/resource_cache.h>

#include <basalt/api/gfx/backend/vertex_layout.h>

#include <basalt/api/scene/scene.h>
#include <basalt/api/scene/system.h>
#include <basalt/api/scene/transform.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/vector2.h>
#include <basalt/api/math/vector3.h>

#include <basalt/api/base/asserts.h>

#include <fmt/format.h>
#include <gsl/span>
#include <imgui.h>

#include <array>
#include <random>
#include <string_view>
#include <utility>
#include <vector>

using namespace basalt;
using namespace std::literals;

namespace {

auto constexpr TEXTURE_FILE_PATH = "data/tribase/Texture.bmp"sv;

auto constexpr INITIAL_NUM_TRIANGLES = u32{1024};

struct Vertex {
  Vector3f32 pos;
  ColorEncoding::A8R8G8B8 color{};
  Vector2f32 uv;

  static auto constexpr sLayout =
    gfx::make_vertex_layout<gfx::VertexElement::Position3F32,
                            gfx::VertexElement::ColorDiffuse1U32A8R8G8B8,
                            gfx::VertexElement::TextureCoords2F32>();
};

using Distribution = std::uniform_real_distribution<float>;

struct TriangleMovement {
  Vector3f32 velocity;
  Vector3f32 rotationVelocity;
};

class TriangleMovementSystem : public System {
  using UpdateBefore = TransformSystem;

public:
  auto on_update(UpdateContext const& ctx) -> void override {
    auto const dt = ctx.deltaTime.count();
    auto& entities = ctx.scene.entity_registry();
    entities.view<Transform, TriangleMovement>().each(
      [&](Transform& transform, TriangleMovement& triangleMovement) {
        transform.position += triangleMovement.velocity * dt;
        transform.rotation += triangleMovement.rotationVelocity * dt;

        if (transform.position.length() > 100.0f) {
          triangleMovement.velocity *= -1.0f;
        }
      });
  }
};

class TexturedTrianglesView final : public View {
public:
  explicit TexturedTrianglesView(Engine& engine, SceneViewPtr sceneView,
                                 SystemId const movementSystemId,
                                 gfx::MaterialHandle const material)
    : mScene{sceneView->scene()}
    , mTrianglesResources{engine.create_gfx_resource_cache()}
    , mMovementSystemId{movementSystemId}
    , mMaterial{material} {
    add_child_bottom(std::move(sceneView));

    // set initial sampler to material
    update_sampler(engine.gfx_context());
    update_triangles();
  }

private:
  ScenePtr mScene;
  gfx::ResourceCachePtr mTrianglesResources;
  std::vector<EntityId> mTriangles;
  gfx::Sampler mSampler;
  u32 mNumTriangles{INITIAL_NUM_TRIANGLES};
  SystemId mMovementSystemId;
  gfx::MaterialHandle mMaterial;
  gfx::TextureFilter mMagFilter{gfx::TextureFilter::Bilinear};
  gfx::TextureFilter mMinFilter{gfx::TextureFilter::Bilinear};
  gfx::TextureMipFilter mMipFilter{gfx::TextureMipFilter::Linear};

  auto update_sampler(gfx::Context& gfxCtx) -> void {
    auto info = gfx::SamplerCreateInfo{mMagFilter, mMinFilter, mMipFilter};

    auto const& gfxInfo = gfxCtx.gfx_info();
    auto const& caps = gfxInfo.adapterInfos[gfxInfo.currentAdapter].deviceCaps;
    BASALT_ASSERT(caps.samplerMinFilterAnisotropic);
    info.maxAnisotropy = caps.samplerMaxAnisotropy;

    mSampler = gfxCtx.create_sampler(info);

    auto& material = gfxCtx.get(mMaterial);
    auto constexpr propertyId = gfx::MaterialPropertyId::SampledTexture;
    auto sampledTexture =
      std::get<gfx::SampledTexture>(*material.get_value(propertyId));
    sampledTexture.sampler = mSampler;
    material.set_value(propertyId, sampledTexture);
  }

  auto update_triangles() -> void {
    auto& entities = mScene->entity_registry();

    entities.destroy(mTriangles.cbegin(), mTriangles.cend());
    mTriangles.clear();
    mTrianglesResources->destroy_all();

    auto randomEngine = std::default_random_engine{std::random_device{}()};
    auto rng2 = Distribution{-1.0f, 1.0f};
    auto const getRandomNormalizedVector = [&] {
      return Vector3f32::normalized(rng2(randomEngine), rng2(randomEngine),
                                    rng2(randomEngine));
    };

    auto const vertexBuffer = [&] {
      auto const vertices = [&] {
        auto rng4 = Distribution{0.0f, 1.0f};
        auto const getRandomColor = [&] {
          return Color::from_non_linear(rng4(randomEngine), rng4(randomEngine),
                                        rng4(randomEngine));
        };
        auto rng5 = Distribution{-1.0f, 2.0f};
        auto const getRandomUv = [&] {
          return Vector2f32{rng5(randomEngine), rng5(randomEngine)};
        };

        auto const numVertices = 3 * mNumTriangles;
        auto vertices = std::vector<Vertex>{};
        vertices.reserve(numVertices);
        for (auto i = u32{0}; i < mNumTriangles; ++i) {
          for (auto vertIdx = u32{0}; vertIdx < 3; ++vertIdx) {
            vertices.push_back(Vertex{
              getRandomNormalizedVector(),
              getRandomColor().to_argb(),
              getRandomUv(),
            });
          }
        }

        return vertices;
      }();

      auto const vertexData = as_bytes(gsl::span{vertices});
      auto info = gfx::VertexBufferCreateInfo{};
      info.sizeInBytes = vertexData.size_bytes();
      info.layout = Vertex::sLayout;

      return mTrianglesResources->create_vertex_buffer(info, vertexData);
    }();

    auto scaleRng = Distribution{1.0f, 5.0f};
    auto rng3 = Distribution{0.1f, 5.0f};
    auto constexpr position = Vector3f32{0.0f, 0.0f, 50.0f};
    auto constexpr rotation = Vector3f32{};
    mTriangles.reserve(mNumTriangles);
    for (auto i = u32{0}; i < mNumTriangles; ++i) {
      auto const scale = scaleRng(randomEngine);
      auto const triangle =
        mScene->create_entity(fmt::format(FMT_STRING("Triangle {}"), i + 1),
                              position, rotation, Vector3f32{scale});

      auto const velocity = rng3(randomEngine) * getRandomNormalizedVector();
      auto const rotationVelocity = Vector3f32{
        rng2(randomEngine),
        rng2(randomEngine),
        rng2(randomEngine),
      };
      triangle.emplace<TriangleMovement>(velocity, rotationVelocity);

      auto const mesh = [&] {
        auto info = gfx::MeshCreateInfo{};
        info.vertexBuffer = vertexBuffer;
        info.vertexStart = 3 * i;
        info.vertexCount = 3;

        return mTrianglesResources->create_mesh(info);
      }();

      triangle.emplace<gfx::Model>(mesh, mMaterial);

      mTriangles.push_back(triangle.entity());
    }
  }

  auto on_update(UpdateContext& ctx) -> void override {
    if (ImGui::IsKeyPressed(ImGuiKey_Space, false)) {
      if (mMovementSystemId) {
        mScene->destroy_system(mMovementSystemId);
        mMovementSystemId = nullhdl;
      } else {
        mMovementSystemId = mScene->create_system<TriangleMovementSystem>();
      }
    }

    if (ImGui::Begin("Settings##TribaseTextures")) {
      ImGui::TextUnformatted(mMovementSystemId ? "Press SPACE to pause"
                                               : "Press SPACE to unpause");

      {
        ImGui::SeparatorText("Number of Triangles");

        auto numTriangles = static_cast<int>(mNumTriangles);
        ImGui::DragInt("##numTriangles", &numTriangles, 4.0f, 0, 1 << 24, "%d",
                       ImGuiSliderFlags_AlwaysClamp);
        mNumTriangles = static_cast<uSize>(numTriangles);

        if (ImGui::Button("Apply")) {
          update_triangles();
        }
      }

      auto const editTextureFilter = [&](gfx::TextureFilter& filter) -> bool {
        auto value = static_cast<int>(filter);

        auto didInteract = ImGui::RadioButton("Point", &value, 0);
        ImGui::SameLine();
        didInteract |= ImGui::RadioButton("Bilinear", &value, 1);
        ImGui::SameLine();
        didInteract |= ImGui::RadioButton("Anisotropic", &value, 2);

        filter = static_cast<gfx::TextureFilter>(value);

        return didInteract;
      };

      auto const editTextureMipFilter =
        [&](gfx::TextureMipFilter& filter) -> bool {
        auto value = static_cast<int>(filter);

        auto didInteract = ImGui::RadioButton("None", &value, 0);
        ImGui::SameLine();
        didInteract |= ImGui::RadioButton("Point", &value, 1);
        ImGui::SameLine();
        didInteract |= ImGui::RadioButton("Linear", &value, 2);

        filter = static_cast<gfx::TextureMipFilter>(value);

        return didInteract;
      };

      auto& gfxCtx = ctx.engine.gfx_context();
      ImGui::SeparatorText("Magnification Filter");
      ImGui::PushID("magFilter");
      if (editTextureFilter(mMagFilter)) {
        update_sampler(gfxCtx);
      }
      ImGui::PopID();

      ImGui::SeparatorText("Minification Filter");
      ImGui::PushID("minFilter");
      if (editTextureFilter(mMinFilter)) {
        update_sampler(gfxCtx);
      }
      ImGui::PopID();

      ImGui::SeparatorText("Mip-Map Filter");
      ImGui::PushID("mipFilter");
      if (editTextureMipFilter(mMipFilter)) {
        update_sampler(gfxCtx);
      }
      ImGui::PopID();
    }
    ImGui::End();
  }
};

} // namespace

auto Benchmarks::make_textured_triangles_view(Engine& engine) -> ViewPtr {
  auto randomEngine = std::default_random_engine{std::random_device{}()};
  auto rng2 = Distribution{-1.0f, 1.0f};
  auto const getRandomNormalizedVector = [&] {
    return Vector3f32::normalized(rng2(randomEngine), rng2(randomEngine),
                                  rng2(randomEngine));
  };

  auto sceneResources = engine.create_gfx_resource_cache();

  auto const material = [&] {
    auto info = gfx::MaterialCreateInfo{};
    info.clazz = [&] {
      auto info = gfx::MaterialClassCreateInfo{};
      auto& pipelineInfo = info.pipelineInfo;
      auto fsInfo = gfx::FixedFragmentShaderCreateInfo{};
      auto constexpr textureStages = std::array{gfx::TextureStage{}};
      fsInfo.textureStages = textureStages;

      pipelineInfo.fragmentShader = &fsInfo;
      pipelineInfo.vertexLayout = Vertex::sLayout;
      pipelineInfo.primitiveType = gfx::PrimitiveType::TriangleList;
      pipelineInfo.depthTest = gfx::TestPassCond::IfLessEqual;
      pipelineInfo.depthWriteEnable = true;

      return sceneResources->create_material_class(info);
    }();

    auto sampledTexture = gfx::SampledTexture{};
    // sampler is set in ::TexturedTrianglesView::update_sampler
    sampledTexture.texture = sceneResources->load_texture_2d(TEXTURE_FILE_PATH);

    auto const values = std::array{
      gfx::MaterialProperty{gfx::MaterialPropertyId::SampledTexture,
                            sampledTexture},
    };
    info.initialValues = values;

    return sceneResources->create_material(info);
  }();

  auto scene = Scene::create();
  auto const movementSystemId = scene->create_system<TriangleMovementSystem>();

  auto& gfxEnv = scene->entity_registry().ctx().emplace<gfx::Environment>();
  gfxEnv.set_background(Color::from_non_linear_rgba8(0, 0, 63));

  auto const camera = scene->create_entity("Camera"s);
  camera.emplace<gfx::Camera>(Vector3f32::forward(), Vector3f32::up(), 90_deg,
                              0.1f, 100.0f);

  auto sceneView = DebugSceneView::create(
    std::move(scene), std::move(sceneResources), camera.entity());

  return std::make_shared<TexturedTrianglesView>(engine, std::move(sceneView),
                                                 movementSystemId, material);
}
