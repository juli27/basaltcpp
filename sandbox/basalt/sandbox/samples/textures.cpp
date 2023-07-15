#include <basalt/sandbox/samples/textures.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>
#include <basalt/api/scene_view.h>

#include <basalt/api/gfx/camera.h>
#include <basalt/api/gfx/resource_cache.h>

#include <basalt/api/scene/ecs.h>
#include <basalt/api/scene/scene.h>
#include <basalt/api/scene/system.h>

#include <basalt/api/shared/log.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/vector3.h>

#include <gsl/span>
#include <imgui/imgui.h>

#include <utility>

using std::array;

using gsl::span;

using namespace basalt::literals;

using basalt::Engine;
using basalt::Entity;
using basalt::EntityId;
using basalt::Scene;
using basalt::SceneView;
using basalt::System;
using basalt::Vector3f32;
using basalt::gfx::Camera;
using basalt::gfx::Material;
using basalt::gfx::MaterialDescriptor;
using basalt::gfx::PrimitiveType;
using basalt::gfx::RenderComponent;
using basalt::gfx::TextureFilter;
using basalt::gfx::TextureMipFilter;
using basalt::gfx::VertexElement;

namespace samples {

namespace {

struct Vertex final {
  array<f32, 3> pos {};
  array<f32, 2> uv {};

  static constexpr array sLayout {VertexElement::Position3F32,
                                  VertexElement::TextureCoords2F32};
};

struct SamplerSettings final {
  array<Material, 9> materials {};
  u32 chosenMaterial {0};
};

class SamplerSettingsSystem final : public System {
public:
  SamplerSettingsSystem() noexcept = default;

  auto on_update(const UpdateContext& ctx) -> void override {
    if (!ImGui::Begin("Sampler Settings")) {
      ImGui::End();

      return;
    }

    bool firstEntity {true};

    ctx.scene.entity_registry().view<RenderComponent, SamplerSettings>().each(
      [&](const EntityId entity, RenderComponent& renderComponent,
          SamplerSettings& samplerSettings) {
        constexpr u8 filterMask {0x3};
        constexpr u8 mipFilterShift {0};
        constexpr u8 minFilterShift {2};

        i32 minFilter {static_cast<i32>(
          samplerSettings.chosenMaterial >> minFilterShift & filterMask)};

        i32 mipFilter {static_cast<i32>(
          samplerSettings.chosenMaterial >> mipFilterShift & filterMask)};

        ImGui::PushID(static_cast<int>(to_integral(entity)));

        if (firstEntity) {
          firstEntity = false;
          ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        }

        if (ImGui::TreeNode("Entity", "Entity %d", to_integral(entity))) {
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

auto add_camera(Scene& scene) -> Entity {
  const Entity camera {scene.create_entity()};
  camera.emplace<Camera>(Vector3f32::forward(), Vector3f32::up(), 90_deg, 0.1f,
                         100.0f);

  return camera;
}

} // namespace

Textures::Textures(Engine& engine)
  : mGfxCache {engine.create_gfx_resource_cache()}
  , mTexture {mGfxCache->load_texture("data/Tiger.bmp")} {
  MaterialDescriptor material;
  material.vertexInputState = Vertex::sLayout;
  material.primitiveType = PrimitiveType::TriangleStrip;
  material.cullBackFace = false;
  material.lit = false;
  material.sampledTexture.texture = mTexture;

  u32 i {0};

  for (const auto filter : {TextureFilter::Point, TextureFilter::Bilinear,
                            TextureFilter::Anisotropic}) {
    material.sampledTexture.filter = filter;

    for (const auto mipFilter :
         {TextureMipFilter::None, TextureMipFilter::Point,
          TextureMipFilter::Linear}) {
      material.sampledTexture.mipFilter = mipFilter;
      mMaterials[i] = mGfxCache->create_material(material);
      ++i;
    }
  }

  array vertices {Vertex {{-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
                  Vertex {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
                  Vertex {{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
                  Vertex {{1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}};
  mMesh = mGfxCache->create_mesh({
    as_bytes(span {vertices}),
    static_cast<u32>(vertices.size()),
    Vertex::sLayout,
  });

  const auto scene {Scene::create()};
  scene->set_background(Color::from_non_linear(0.103f, 0.103f, 0.103f));

  const Entity quad {scene->create_entity({0.0f, 0.0f, 1.5f})};
  quad.emplace<RenderComponent>(mMesh, std::get<0>(mMaterials));
  quad.emplace<SamplerSettings>(mMaterials, 0u);

  const Entity camera {add_camera(*scene)};

  scene->create_system<SamplerSettingsSystem>();
  add_child_top(SceneView::create(scene, mGfxCache, camera.entity()));
}

} // namespace samples
