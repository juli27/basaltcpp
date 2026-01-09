#include "samples.h"

#include <basalt/sandbox/shared/debug_scene_view.h>

#include <basalt/api/debug_ui.h>
#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/context.h>
#include <basalt/api/gfx/camera.h>
#include <basalt/api/gfx/environment.h>
#include <basalt/api/gfx/material.h>
#include <basalt/api/gfx/mesh.h>
#include <basalt/api/gfx/resource_cache.h>

#include <basalt/api/gfx/backend/buffer.h>
#include <basalt/api/gfx/backend/vertex_layout.h>

#include <basalt/api/scene/scene.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/vector2.h>
#include <basalt/api/math/vector3.h>

#include <basalt/api/base/asserts.h>

#include <fmt/format.h>
#include <gsl/span>
#include <imgui.h>

#include <array>
#include <memory>
#include <string>
#include <utility>

using namespace basalt;
using namespace std::literals;

namespace {

struct Vertex {
  Vector3f32 pos;
  ColorEncoding::A8R8G8B8 diffuse{};
  Vector2f32 uv;

  static constexpr auto LAYOUT =
    gfx::make_vertex_layout<gfx::VertexElement::Position3F32,
                            gfx::VertexElement::ColorDiffuse1U32A8R8G8B8,
                            gfx::VertexElement::TextureCoords2F32>();
};

class FogView final : public View {
public:
  FogView(Engine const& engine, gfx::SamplerHandle const sampler,
          std::array<gfx::TextureHandle, 6> const& textures,
          std::array<EntityId, 6> const& entities, SceneViewPtr sceneView)
    : mScene{sceneView->scene()}
    , mMaterialCache{engine.create_gfx_resource_cache()}
    , mSampler{sampler}
    , mTextures{textures}
    , mEntities{entities} {
    add_child_bottom(std::move(sceneView));

    auto& gfxEnv = mScene->entity_registry().ctx().get<gfx::Environment>();
    gfxEnv.set_background(mFogColor);

    update_material_class();
  }

private:
  ScenePtr mScene;
  gfx::ResourceCachePtr mMaterialCache;
  gfx::SamplerHandle mSampler;
  std::array<gfx::TextureHandle, 6> mTextures{};
  std::array<gfx::MaterialHandle, 6> mMaterials{};
  std::array<EntityId, 6> mEntities{};
  gfx::FogMode mFogMode{gfx::FogMode::Linear};
  bool mVertexFogRangeBased{true};
  bool mFragmentFog{true};
  Color mFogColor{Color::from_non_linear(0.5f, 0.5f, 0.5f)};
  f32 mFogStart{10.0f};
  f32 mFogEnd{60.0f};
  f32 mFogDensity{0.025f};

  auto update_materials(gfx::Context& gfxCtx) -> void {
    for (auto const materialHdl : std::as_const(mMaterials)) {
      auto& material = gfxCtx.get(materialHdl);
      material.set_value(
        gfx::MaterialPropertyId::FogParameters,
        gfx::FogParameters{mFogColor, mFogStart, mFogEnd, mFogDensity});
    }
  }

  auto update_material_class() -> void {
    mMaterialCache->destroy_all();

    auto const materialClass = [&] {
      auto info = gfx::MaterialClassCreateInfo{};
      auto& pipelineInfo = info.pipelineInfo;

      auto vs = gfx::FixedVertexShaderCreateInfo{};
      vs.fog = mFogMode;
      vs.fogRangeBased = mVertexFogRangeBased;

      auto constexpr textureStages = std::array{gfx::TextureStage{}};
      auto const fogMode = mFragmentFog ? mFogMode : gfx::FogMode::None;
      auto const fs =
        gfx::FixedFragmentShaderCreateInfo{textureStages, fogMode};

      pipelineInfo.vertexShader = &vs;
      pipelineInfo.fragmentShader = &fs;
      pipelineInfo.vertexLayout = Vertex::LAYOUT;
      pipelineInfo.primitiveType = gfx::PrimitiveType::TriangleStrip;
      pipelineInfo.cullMode = gfx::CullMode::CounterClockwise;
      pipelineInfo.depthTest = gfx::TestPassCond::IfLessEqual;
      pipelineInfo.depthWriteEnable = true;

      return mMaterialCache->create_material_class(info);
    }();

    BASALT_ASSERT(mTextures.size() >= mMaterials.size() &&
                  mEntities.size() >= mMaterials.size());
    for (auto i = uSize{0}; i < mMaterials.size(); ++i) {
      auto materialInfo = gfx::MaterialCreateInfo{};
      materialInfo.clazz = materialClass;

      auto const properties = std::array{
        gfx::MaterialProperty{
          gfx::MaterialPropertyId::SampledTexture,
          gfx::SampledTexture{mSampler, mTextures[i]},
        },
        gfx::MaterialProperty{
          gfx::MaterialPropertyId::FogParameters,
          gfx::FogParameters{mFogColor, mFogStart, mFogEnd, mFogDensity},
        },
      };
      materialInfo.initialValues = properties;
      auto const material = mMaterialCache->create_material(materialInfo);
      mMaterials[i] = material;

      auto const entityId = mEntities[i];
      auto& gfxModel = mScene->entity_registry().get<gfx::Model>(entityId);
      gfxModel.material = material;
    }
  }

  auto on_update(UpdateContext& ctx) -> void override {
    ImGui::SetNextWindowSize(ImVec2{300.0f, 0}, ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Settings##Fog")) {
      ImGui::End();
      return;
    }

    if (ImGui::RadioButton("vertex fog",
                           !mFragmentFog && !mVertexFogRangeBased)) {
      mVertexFogRangeBased = false;
      mFragmentFog = false;
      update_material_class();
    }
    if (ImGui::RadioButton("vertex fog (range-based)",
                           !mFragmentFog && mVertexFogRangeBased)) {
      mVertexFogRangeBased = true;
      mFragmentFog = false;
      update_material_class();
    }
    if (ImGui::RadioButton("fragment fog", mFragmentFog)) {
      mFragmentFog = true;
      update_material_class();
    }

    ImGui::Separator();

    if (DebugUi::edit_color3("Fog color", mFogColor)) {
      auto& gfxEnv = mScene->entity_registry().ctx().get<gfx::Environment>();
      gfxEnv.set_background(mFogColor);
      update_materials(ctx.engine.gfx_context());
    }

    ImGui::Separator();

    if (ImGui::RadioButton("linear", mFogMode == gfx::FogMode::Linear)) {
      mFogMode = gfx::FogMode::Linear;
      update_material_class();
    }
    if (ImGui::RadioButton("exponential",
                           mFogMode == gfx::FogMode::Exponential)) {
      mFogMode = gfx::FogMode::Exponential;
      update_material_class();
    }
    if (ImGui::RadioButton("exponential squared",
                           mFogMode == gfx::FogMode::ExponentialSquared)) {
      mFogMode = gfx::FogMode::ExponentialSquared;
      update_material_class();
    }

    ImGui::BeginDisabled(mFogMode != gfx::FogMode::Linear);
    if (ImGui::DragFloatRange2("range", &mFogStart, &mFogEnd, 1, 0.0f,
                               100.0f)) {
      update_materials(ctx.engine.gfx_context());
    }
    ImGui::EndDisabled();

    ImGui::BeginDisabled(mFogMode == gfx::FogMode::Linear);
    if (ImGui::DragFloat("density", &mFogDensity, 0.005f, 0.0f, 1.0f, "%.3f",
                         ImGuiSliderFlags_AlwaysClamp)) {
      update_materials(ctx.engine.gfx_context());
    }
    ImGui::EndDisabled();

    ImGui::End();
  }
};

} // namespace

auto Samples::new_fog_sample(Engine& engine) -> ViewPtr {
  auto scene = Scene::create();

  auto gfxCache = engine.create_gfx_resource_cache();
  auto const sampler = [&] {
    auto info = gfx::SamplerCreateInfo{};
    info.magFilter = gfx::TextureFilter::Bilinear;
    info.minFilter = gfx::TextureFilter::Bilinear;
    info.mipFilter = gfx::TextureMipFilter::Linear;

    return gfxCache->create_sampler(info);
  }();

  auto textures = std::array<gfx::TextureHandle, 6>{};
  for (auto i = uSize{0}; i < textures.size(); ++i) {
    auto const fileName =
      fmt::format(FMT_STRING("data/tribase/02-06_fog/Texture{}.bmp"), i + 1);
    textures[i] = gfxCache->load_texture_2d(fileName);
  }

  auto const meshHandle = [&] {
    auto info = gfx::MeshCreateInfo{};
    auto constexpr vertices = std::array{
      Vertex{Vector3f32{-1.0f, -1.0f, 0.0f},
             ColorEncoding::pack_a8r8g8b8_u32(191, 255, 255),
             Vector2f32{0.0f, 1.0f}},
      Vertex{Vector3f32{-1.0f, 1.0f, 0.0f},
             ColorEncoding::pack_a8r8g8b8_u32(255, 191, 255),
             Vector2f32{0.0f, 0.0f}},
      Vertex{Vector3f32{1.0f, -1.0f, 0.0f},
             ColorEncoding::pack_a8r8g8b8_u32(255, 255, 191),
             Vector2f32{1.0f, 1.0f}},
      Vertex{Vector3f32{1.0f, 1.0f, 0.0f},
             ColorEncoding::pack_a8r8g8b8_u32(255, 255, 255),
             Vector2f32{1.0f, 0.0f}},
    };
    auto const vertexData = as_bytes(gsl::span{vertices});
    info.vertexBuffer = gfxCache->create_vertex_buffer(
      gfx::VertexBufferCreateInfo{vertexData.size_bytes(), Vertex::LAYOUT},
      vertexData);
    info.vertexCount = static_cast<u32>(vertices.size());

    return gfxCache->create_mesh(info);
  }();

  auto entities = std::array<EntityId, 6>{};
  // create signs
  for (auto i = u32{0}; i <= 4; ++i) {
    auto name = fmt::format(FMT_STRING("Sign {}"), i + 1);
    auto constexpr scale = Vector3f32{7.5f};
    auto const posX = 5.0f * static_cast<f32>(i * i) - 10.0f;
    auto const posZ = static_cast<f32>(i + 1) * 10.0f;
    auto const position = Vector3f32{posX, 0.0f, posZ};

    auto const sign =
      scene->create_entity(std::move(name), position, Vector3f32{}, scale);
    // material is set by FogView
    sign.emplace<gfx::Model>(meshHandle, nullhdl);
    entities[i] = sign;
  }

  auto const ground = scene->create_entity(
    "Ground"s, Vector3f32{0.0f, -7.5f, 75.0f},
    Vector3f32{(90_deg).radians(), 0.0f, 0.0f}, Vector3f32{75.0f});
  // material is set by FogView
  ground.emplace<gfx::Model>(meshHandle, nullhdl);
  std::get<5>(entities) = ground;

  auto const camera = scene->create_entity("Camera"s);
  camera.emplace<gfx::Camera>(Vector3f32::forward(), Vector3f32::up(), 120_deg,
                              0.1f, 100.0f);

  auto sceneView = DebugSceneView::create(std::move(scene), std::move(gfxCache),
                                          camera.entity());

  return std::make_shared<FogView>(engine, sampler, textures, entities,
                                   std::move(sceneView));
}
