#include <basalt/sandbox/samples/textures.h>

#include <basalt/api/engine.h>
#include <basalt/api/prelude.h>
#include <basalt/api/scene_view.h>

#include <basalt/api/gfx/camera.h>
#include <basalt/api/gfx/resource_cache.h>

#include <basalt/api/scene/scene.h>

#include <basalt/api/shared/log.h>

#include <basalt/api/math/angle.h>
#include <basalt/api/math/vector3.h>

#include <gsl/span>
#include <imgui/imgui.h>

using std::array;

using namespace entt::literals;
using gsl::span;

using namespace basalt::literals;

using basalt::Color;
using basalt::Engine;
using basalt::Scene;
using basalt::SceneView;
using basalt::Vector3f32;
using basalt::gfx::Camera;
using basalt::gfx::Material;
using basalt::gfx::MaterialDescriptor;
using basalt::gfx::MeshDescriptor;
using basalt::gfx::PrimitiveType;
using basalt::gfx::RenderComponent;
using basalt::gfx::Texture;
using basalt::gfx::TextureFilter;
using basalt::gfx::TextureMipFilter;
using basalt::gfx::VertexElement;
using basalt::gfx::VertexLayout;

namespace samples {

namespace {

auto create_camera() -> Camera {
  return Camera {Vector3f32 {0.0f},
                 Vector3f32::forward(),
                 Vector3f32::up(),
                 90.0_deg,
                 0.1f,
                 100.0f};
}

} // namespace

Textures::Textures(Engine& engine)
  : mScene {std::make_shared<Scene>()}
  , mSceneView {std::make_shared<SceneView>(mScene, create_camera())} {
  add_child_top(mSceneView);

  mScene->set_background(Color::from_non_linear(0.103f, 0.103f, 0.103f));

  auto& gfxResourceCache {engine.gfx_resource_cache()};

  const VertexLayout vertexLayout {VertexElement::Position3F32,
                                   VertexElement::TextureCoords2F32};

  MaterialDescriptor material {};
  material.vertexInputState = vertexLayout;
  material.primitiveType = PrimitiveType::TriangleStrip;
  material.cullBackFace = false;
  material.lit = false;
  material.sampledTexture.texture =
    engine.get_or_load<Texture>("data/Tiger.bmp"_hs);

  u32 i {0};

  for (const auto filter : {TextureFilter::Point, TextureFilter::Linear,
                            TextureFilter::LinearAnisotropic}) {
    material.sampledTexture.filter = filter;

    for (const auto mipFilter :
         {TextureMipFilter::None, TextureMipFilter::Point,
          TextureMipFilter::Linear}) {
      material.sampledTexture.mipFilter = mipFilter;
      mMaterials[i] = gfxResourceCache.create_material(material);
      ++i;
    }
  }

  struct Vertex final {
    f32 x;
    f32 y;
    f32 z;
    f32 u;
    f32 v;
  };

  array<Vertex, 4> vertices {
    Vertex {-1.0f, 1.0f, 0.0f, 0.0f, 0.0f},
    Vertex {1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
    Vertex {-1.0f, -1.0f, 0.0f, 0.0f, 1.0f},
    Vertex {1.0f, -1.0f, 0.0f, 1.0f, 1.0f},
  };

  const MeshDescriptor mesh {as_bytes(span {vertices}),
                             static_cast<u32>(vertices.size()), vertexLayout};

  mQuad = mScene->create_entity(Vector3f32 {0.0f, 0.0f, 1.5f});
  auto& rc {mQuad.emplace<RenderComponent>()};
  rc.mesh = engine.gfx_resource_cache().create_mesh(mesh);
  rc.material = std::get<0>(mMaterials);
}

void Textures::on_tick(Engine&) {
  constexpr u8 filterMask {0x3};
  constexpr u8 mipFilterShift {0};
  constexpr u8 minFilterShift {2};

  i32 minFilter {
    static_cast<i32>(mChosenMaterial >> minFilterShift & filterMask)};

  i32 mipFilter {
    static_cast<i32>(mChosenMaterial >> mipFilterShift & filterMask)};

  if (ImGui::Begin("Settings##SamplesTextures")) {
    ImGui::TextUnformatted("Filter");
    ImGui::PushID("Filter");
    ImGui::RadioButton("Point", &minFilter, 0);
    ImGui::SameLine();
    ImGui::RadioButton("Linear", &minFilter, 1);
    ImGui::SameLine();
    ImGui::RadioButton("Linear (anisotropic)", &minFilter, 2);
    ImGui::PopID();

    ImGui::TextUnformatted("MIP Filter");
    ImGui::PushID("Mip");
    ImGui::RadioButton("None", &mipFilter, 0);
    ImGui::SameLine();
    ImGui::RadioButton("Point", &mipFilter, 1);
    ImGui::SameLine();
    ImGui::RadioButton("Linear", &mipFilter, 2);
    ImGui::PopID();
  }

  mChosenMaterial = static_cast<u32>(minFilter) << minFilterShift |
                    static_cast<u32>(mipFilter) << mipFilterShift;

  ImGui::End();

  auto& rc {mQuad.get<RenderComponent>()};
  rc.material = mMaterials[minFilter * 3 + mipFilter];
}

} // namespace samples
