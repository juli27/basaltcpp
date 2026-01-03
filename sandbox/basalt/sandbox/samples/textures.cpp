#include "samples.h"

#include <basalt/sandbox/shared/debug_scene_view.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/camera.h>
#include <basalt/api/gfx/environment.h>
#include <basalt/api/gfx/info.h>
#include <basalt/api/gfx/material.h>
#include <basalt/api/gfx/material_class.h>
#include <basalt/api/gfx/mesh.h>
#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/backend/buffer.h>
#include <basalt/api/gfx/backend/vertex_layout.h>

#include <basalt/api/scene/ecs.h>
#include <basalt/api/scene/scene.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/vector2.h>
#include <basalt/api/math/vector3.h>

#include <gsl/span>
#include <imgui.h>

#include <array>
#include <string_view>
#include <utility>

using namespace basalt;
using namespace std::literals;

namespace {

auto constexpr TEXTURE_FILE_PATH = "data/Tiger.bmp"sv;

struct Vertex {
  Vector3f32 pos;
  Vector2f32 uv;

  static auto constexpr sLayout =
    gfx::make_vertex_layout<gfx::VertexElement::Position3F32,
                            gfx::VertexElement::TextureCoords2F32>();
};

auto constexpr QUAD_VERTICES = std::array{
  Vertex{{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
  Vertex{{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
  Vertex{{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
  Vertex{{1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
};

class TexturesView final : public View {
public:
  explicit TexturesView(gfx::Context& gfxCtx, SceneViewPtr sceneView,
                        gfx::MaterialHandle const material,
                        gfx::VertexBufferHandle const vertexBuffer)
    : mMaterial{material}
    , mVertexBuffer{vertexBuffer} {
    add_child_bottom(std::move(sceneView));

    update_sampler(gfxCtx);
  }

private:
  gfx::Sampler mSampler;
  gfx::MaterialHandle mMaterial;
  gfx::VertexBufferHandle mVertexBuffer;
  SecondsF32 mTime{0s};
  i32 mUvAnimationMode{0};
  gfx::TextureFilter mFilter{gfx::TextureFilter::Point};
  gfx::TextureMipFilter mMipFilter{gfx::TextureMipFilter::None};
  bool mAnimateUvs{false};

  auto update_sampler(gfx::Context& gfxCtx) -> void {
    auto info = gfx::SamplerCreateInfo{};
    info.magFilter = mFilter;
    info.minFilter = mFilter;
    info.mipFilter = mMipFilter;

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

  auto on_update(UpdateContext& ctx) -> void override {
    auto& gfxCtx = ctx.engine.gfx_context();

    if (ImGui::Begin("Settings##Textures")) {
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

      ImGui::SeparatorText("Filter");
      ImGui::PushID("Filter");
      if (editTextureFilter(mFilter)) {
        update_sampler(gfxCtx);
      }
      ImGui::PopID();

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

      ImGui::SeparatorText("Mip-Map Filter");
      ImGui::PushID("MipFilter");
      if (editTextureMipFilter(mMipFilter)) {
        update_sampler(gfxCtx);
      }
      ImGui::PopID();

      ImGui::Checkbox("Animate UVs", &mAnimateUvs);
      ImGui::SameLine();
      if (ImGui::Button("Reset")) {
        mTime = 0s;
        gfxCtx.with_mapping_of(
          mVertexBuffer, [](gsl::span<std::byte> const vbData) {
            auto const vertexData = as_bytes(gsl::span{QUAD_VERTICES});

            std::copy_n(vertexData.begin(),
                        std::min(vertexData.size_bytes(), vbData.size_bytes()),
                        vbData.begin());
          });
      }

      ImGui::Indent();
      if (ImGui::RadioButton("Move", &mUvAnimationMode, 0)) {
        mTime = 0s;
      }
      if (ImGui::RadioButton("Rotate", &mUvAnimationMode, 1)) {
        mTime = 0s;
      }
    }

    ImGui::End();

    if (mAnimateUvs) {
      mTime += ctx.deltaTime;
    }
    auto const t = mTime.count();

    if (mUvAnimationMode == 0) {
      gfxCtx.with_mapping_of(
        mVertexBuffer, [&](gsl::span<std::byte> const vbData) {
          auto const vertexData =
            gsl::span<Vertex>{reinterpret_cast<Vertex*>(vbData.data()),
                              vbData.size() / sizeof(Vertex)};

          auto const translateUv = [&](Vertex const& v) {
            return Vertex{v.pos, Vector2f32{
                                   v.uv.x() + 0.25f * t,
                                   v.uv.y(),
                                 }};
          };
          std::transform(QUAD_VERTICES.begin(), QUAD_VERTICES.end(),
                         vertexData.begin(), translateUv);
        });
    } else if (mUvAnimationMode == 1) {
      gfxCtx.with_mapping_of(
        mVertexBuffer, [&](gsl::span<std::byte> const vbData) {
          auto const vertexData =
            gsl::span<Vertex>{reinterpret_cast<Vertex*>(vbData.data()),
                              vbData.size() / sizeof(Vertex)};

          auto const rotation =
            Matrix4x4f32::rotation_z(Angle::degrees(45.0f * t));
          auto const rotateUv = [&](Vertex const& v) {
            return Vertex{v.pos,
                          Vector2f32{
                            v.uv.dot(Vector2f32{rotation.m11, rotation.m21}),
                            v.uv.dot(Vector2f32{rotation.m12, rotation.m22}),
                          }};
          };
          std::transform(QUAD_VERTICES.begin(), QUAD_VERTICES.end(),
                         vertexData.begin(), rotateUv);
        });
    }
  }
};

} // namespace

// TODO: when supporting dynamic buffers: texture coordinate modification and
// address mode demo
auto Samples::new_textures_sample(Engine& engine) -> ViewPtr {
  auto gfxCache = engine.create_gfx_resource_cache();

  auto const vertexBufferHdl = [&] {
    auto const vertexData = as_bytes(gsl::span{QUAD_VERTICES});

    auto info = gfx::VertexBufferCreateInfo{};
    info.sizeInBytes = vertexData.size_bytes();
    info.layout = Vertex::sLayout;

    return gfxCache->create_vertex_buffer(info, vertexData);
  }();

  auto const meshHdl = [&] {
    auto info = gfx::MeshCreateInfo{};
    info.vertexBuffer = vertexBufferHdl;
    info.vertexCount = static_cast<u32>(QUAD_VERTICES.size());

    return gfxCache->create_mesh(info);
  }();

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
      pipelineInfo.primitiveType = gfx::PrimitiveType::TriangleStrip;

      return gfxCache->create_material_class(info);
    }();

    // sampler is set in ::TexturesView::update_sampler
    auto sampledTexture = gfx::SampledTexture{};
    sampledTexture.texture = gfxCache->load_texture_2d(TEXTURE_FILE_PATH);

    auto const properties = std::array{
      gfx::MaterialProperty{gfx::MaterialPropertyId::SampledTexture,
                            sampledTexture},
    };
    info.initialValues = properties;

    return gfxCache->create_material(info);
  }();

  auto scene = Scene::create();

  auto& gfxEnv = scene->entity_registry().ctx().emplace<gfx::Environment>();
  gfxEnv.set_background(Color::from_non_linear(0.103f, 0.103f, 0.103f));

  auto const quad = scene->create_entity(Vector3f32{0.0f, 0.0f, 1.5f});
  quad.emplace<gfx::Model>(meshHdl, materialHdl);

  auto const camera = scene->create_entity();
  camera.emplace<gfx::Camera>(Vector3f32::forward(), Vector3f32::up(), 90_deg,
                              0.1f, 100.0f);

  auto sceneView = DebugSceneView::create(std::move(scene), std::move(gfxCache),
                                          camera.entity());

  return std::make_shared<TexturesView>(
    engine.gfx_context(), std::move(sceneView), materialHdl, vertexBufferHdl);
}
