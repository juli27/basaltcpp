#include <basalt/sandbox/samples/samples.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>
#include <basalt/api/scene_view.h>

#include <basalt/api/gfx/camera.h>
#include <basalt/api/gfx/environment.h>
#include <basalt/api/gfx/resource_cache.h>
#include <basalt/api/gfx/backend/vertex_layout.h>

#include <basalt/api/scene/ecs.h>
#include <basalt/api/scene/scene.h>
#include <basalt/api/scene/system.h>

#include <basalt/api/shared/log.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/vector2.h>
#include <basalt/api/math/vector3.h>

#include <gsl/span>
#include <imgui/imgui.h>

#include <array>
#include <string_view>
#include <utility>

using namespace std::literals;
using std::array;

using gsl::span;

using namespace basalt::literals;

using basalt::Engine;
using basalt::Entity;
using basalt::EntityId;
using basalt::Scene;
using basalt::SceneView;
using basalt::System;
using basalt::Vector2f32;
using basalt::Vector3f32;
using basalt::ViewPtr;
using basalt::gfx::Camera;
using basalt::gfx::Environment;
using basalt::gfx::FixedFragmentShaderCreateInfo;
using basalt::gfx::MaterialCreateInfo;
using basalt::gfx::MaterialHandle;
using basalt::gfx::PipelineCreateInfo;
using basalt::gfx::PrimitiveType;
using basalt::gfx::RenderComponent;
using basalt::gfx::SamplerCreateInfo;
using basalt::gfx::TestPassCond;
using basalt::gfx::TextureFilter;
using basalt::gfx::TextureMipFilter;
using basalt::gfx::TextureStage;
using basalt::gfx::VertexElement;

namespace {

constexpr auto TEXTURE_FILE_PATH = "data/Tiger.bmp"sv;

struct Vertex final {
  Vector3f32 pos{};
  Vector2f32 uv{};

  static constexpr auto sLayout =
    basalt::gfx::make_vertex_layout<VertexElement::Position3F32,
                                    VertexElement::TextureCoords2F32>();
};

struct SamplerSettings final {
  array<MaterialHandle, 9> materials{};
  u32 chosenMaterial{0};
};

class SamplerSettingsSystem final : public System {
public:
  using UpdateBefore = basalt::gfx::GfxSystem;

  SamplerSettingsSystem() noexcept = default;

  auto on_update(UpdateContext const& ctx) -> void override {
    if (!ImGui::Begin("Sampler Settings")) {
      ImGui::End();

      return;
    }

    auto firstEntity = true;

    ctx.scene.entity_registry().view<RenderComponent, SamplerSettings>().each(
      [&](EntityId const entityId, RenderComponent& renderComponent,
          SamplerSettings& samplerSettings) {
        constexpr auto filterMask = u8{0x3};
        constexpr auto mipFilterShift = u8{0};
        constexpr auto minFilterShift = u8{2};

        auto minFilter = static_cast<i32>(
          samplerSettings.chosenMaterial >> minFilterShift & filterMask);

        auto mipFilter = static_cast<i32>(
          samplerSettings.chosenMaterial >> mipFilterShift & filterMask);

        ImGui::PushID(static_cast<int>(to_integral(entityId)));

        if (firstEntity) {
          firstEntity = false;
          ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        }

        if (ImGui::TreeNode("Entity", "Entity %d", to_integral(entityId))) {
          ImGui::TextUnformatted("Filter");
          ImGui::PushID("Filter");
          ImGui::RadioButton("Point", &minFilter, 0);
          ImGui::SameLine();
          ImGui::RadioButton("Bilinear", &minFilter, 1);
          ImGui::SameLine();
          ImGui::RadioButton("Anisotropic", &minFilter, 2);
          ImGui::PopID();

          ImGui::TextUnformatted("MIP Filter");
          ImGui::PushID("Mip");
          ImGui::RadioButton("None", &mipFilter, 0);
          ImGui::SameLine();
          ImGui::RadioButton("Point", &mipFilter, 1);
          ImGui::SameLine();
          ImGui::RadioButton("Linear", &mipFilter, 2);
          ImGui::PopID();

          ImGui::TreePop();
        }

        ImGui::PopID();

        samplerSettings.chosenMaterial =
          static_cast<u32>(minFilter) << minFilterShift |
          static_cast<u32>(mipFilter) << mipFilterShift;

        renderComponent.material =
          samplerSettings.materials[minFilter * 3 + mipFilter];
      });

    ImGui::End();
  }
};

} // namespace

// TODO: when supporting dynamic buffers: texture coordinate modification and
// address mode demo
auto Samples::new_textures_sample(Engine& engine) -> ViewPtr {
  auto gfxCache = engine.create_gfx_resource_cache();

  constexpr auto vertices = array{Vertex{{-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
                                  Vertex{{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
                                  Vertex{{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
                                  Vertex{{1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}};
  auto const mesh = gfxCache->create_mesh({
    as_bytes(span{vertices}),
    static_cast<u32>(vertices.size()),
    Vertex::sLayout,
  });

  auto scene = Scene::create();
  auto& gfxEnv = scene->entity_registry().ctx().emplace<Environment>();
  gfxEnv.set_background(Color::from_non_linear(0.103f, 0.103f, 0.103f));

  auto const quad = scene->create_entity(Vector3f32{0.0f, 0.0f, 1.5f});
  // material is set in SamplerSettingsSystem
  quad.emplace<RenderComponent>(mesh, MaterialHandle::null());

  auto& samplerSettings = quad.emplace<SamplerSettings>();

  auto materialDesc = MaterialCreateInfo{};
  materialDesc.pipeline = [&] {
    auto fs = FixedFragmentShaderCreateInfo{};
    constexpr auto textureStages = array{TextureStage{}};
    fs.textureStages = textureStages;

    auto pipelineDesc = PipelineCreateInfo{};
    pipelineDesc.fragmentShader = &fs;
    pipelineDesc.vertexLayout = Vertex::sLayout;
    pipelineDesc.primitiveType = PrimitiveType::TriangleStrip;
    pipelineDesc.depthTest = TestPassCond::IfLessEqual;
    pipelineDesc.depthWriteEnable = true;

    return gfxCache->create_pipeline(pipelineDesc);
  }();
  materialDesc.sampledTexture.texture =
    gfxCache->load_texture_2d(TEXTURE_FILE_PATH);

  auto const maxAnisotropy =
    engine.gfx_info().currentDeviceCaps.samplerMaxAnisotropy;
  auto& materials = samplerSettings.materials;
  auto i = u32{0};
  for (auto const filter : {TextureFilter::Point, TextureFilter::Bilinear,
                            TextureFilter::Anisotropic}) {
    for (auto const mipFilter :
         {TextureMipFilter::None, TextureMipFilter::Point,
          TextureMipFilter::Linear}) {
      auto samplerDesc = SamplerCreateInfo{filter, filter, mipFilter};
      samplerDesc.maxAnisotropy =
        filter == TextureFilter::Anisotropic ? maxAnisotropy : 1;
      materialDesc.sampledTexture.sampler =
        gfxCache->create_sampler(samplerDesc);

      materials[i] = gfxCache->create_material(materialDesc);
      ++i;
    }
  }

  scene->create_system<SamplerSettingsSystem>();

  auto const cameraId = [&] {
    auto const camera = scene->create_entity();
    camera.emplace<Camera>(Vector3f32::forward(), Vector3f32::up(), 90_deg,
                           0.1f, 100.0f);

    return camera.entity();
  }();

  return SceneView::create(std::move(scene), std::move(gfxCache), cameraId);
}
